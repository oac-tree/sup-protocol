/******************************************************************************
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "polling_timeout_handler.h"

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/async_invocation.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/protocol_rpc_client.h>

namespace sup
{
namespace protocol
{
namespace
{
std::pair<bool, sup::dto::AnyValue> TryGetPayload(const sup::dto::AnyValue& reply);
ProtocolResult HandleSyncInvokeReply(const sup::dto::AnyValue& reply, sup::dto::AnyValue& output);
}  // unnamed namespace

ProtocolRPCClient::ProtocolRPCClient(sup::dto::AnyFunctor& any_functor, PayloadEncoding encoding)
    : m_any_functor{any_functor}, m_config{encoding}
{
}

ProtocolRPCClient::ProtocolRPCClient(sup::dto::AnyFunctor& any_functor,
                                     ProtocolRPCClientConfig config)
    : m_any_functor{any_functor}, m_config{config}
{
}

ProtocolRPCClient::~ProtocolRPCClient() = default;

ProtocolResult ProtocolRPCClient::Invoke(const sup::dto::AnyValue& input,
                                         sup::dto::AnyValue& output)
{
  if (sup::dto::IsEmptyValue(input))
  {
    return ClientTransportEncodingError;
  }
  if (m_config.m_async)
  {
    return HandleAsyncInvoke(input, output);
  }
  return HandleSyncInvoke(input, output);
}

ProtocolResult ProtocolRPCClient::Service(const sup::dto::AnyValue& input,
                                          sup::dto::AnyValue& output)
{
  if (sup::dto::IsEmptyValue(input))
  {
    return ClientTransportEncodingError;
  }
  auto request = utils::CreateServiceRequest(input, m_config.m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_any_functor(request);
  }
  catch (...)
  {
    return ClientTransportException;
  }
  if (!utils::CheckServiceReplyFormat(reply))
  {
    return ClientTransportDecodingError;
  }
  auto result_info = utils::TryExtractProtocolResult(reply);
  if (!result_info.first)
  {
    return ClientTransportDecodingError;
  }
  auto result = result_info.second;
  if (result == Success)
  {
    auto payload_info = TryGetPayload(reply);
    if (!payload_info.first)
    {
      return ClientTransportDecodingError;
    }
    const auto& payload = payload_info.second;
    if (!sup::dto::IsEmptyValue(payload) && !sup::dto::TryAssignIfEmptyOrConvert(output, payload))
    {
      return ClientTransportDecodingError;
    }
  }
  return result;
}

ProtocolResult ProtocolRPCClient::HandleSyncInvoke(const sup::dto::AnyValue& input,
                                                   sup::dto::AnyValue& output)
{
  auto request = utils::CreateRPCRequest(input, m_config.m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_any_functor(request);
  }
  catch (...)
  {
    return ClientTransportException;
  }
  return HandleSyncInvokeReply(reply, output);
}

ProtocolResult ProtocolRPCClient::HandleAsyncInvoke(const sup::dto::AnyValue& input,
                                                    sup::dto::AnyValue& output)
{
  AsyncInvocation invocation{m_any_functor, m_config.m_encoding};
  auto start_result = invocation.Start(input);
  if (start_result != Success)
  {
    return start_result;
  }
  if (!invocation.IsSynchronous())
  {
    PollingTimeoutHandler polling_handler{m_config.m_timeout_sec, m_config.m_polling_interval_sec};
    while (true)
    {
      auto poll_result = invocation.PollOnce();
      if (poll_result.first != Success)
      {
        return poll_result.first;
      }
      if (poll_result.second)
      {
        break;
      }
      if (!polling_handler.Wait())
      {
        return AsynchronousProtocolTimeout;
      }
    }
  }
  return invocation.GetReply(output);
}

namespace
{

std::pair<bool, sup::dto::AnyValue> TryGetPayload(const sup::dto::AnyValue& reply)
{
  std::pair<bool, sup::dto::AnyValue> failure{false, {}};
  if (!reply.HasField(constants::REPLY_PAYLOAD))
  {
    return {true, {}};
  }
  auto encoding_info = utils::TryGetPacketEncoding(reply);
  if (!encoding_info.first)
  {
    return failure;
  }
  auto encoding = encoding_info.second;
  auto payload_result = utils::TryExtractRPCReplyPayload(reply, encoding);
  if (!payload_result.first)
  {
    return failure;
  }
  return {true, payload_result.second};
}

ProtocolResult HandleSyncInvokeReply(const sup::dto::AnyValue& reply, sup::dto::AnyValue& output)
{
  if (!utils::CheckReplyFormat(reply))
  {
    return ClientTransportDecodingError;
  }
  auto result_info = utils::TryExtractProtocolResult(reply);
  if (!result_info.first)
  {
    return ClientTransportDecodingError;
  }
  auto result = result_info.second;
  if (result == Success)
  {
    auto payload_info = TryGetPayload(reply);
    if (!payload_info.first)
    {
      return ClientTransportDecodingError;
    }
    const auto& payload = payload_info.second;
    if (!sup::dto::IsEmptyValue(payload) && !sup::dto::TryAssignIfEmptyOrConvert(output, payload))
    {
      return ClientTransportDecodingError;
    }
  }
  return result;
}

}  // unnamed namespace

}  // namespace protocol

}  // namespace sup
