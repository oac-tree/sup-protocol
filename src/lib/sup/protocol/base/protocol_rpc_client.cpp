/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include <sup/protocol/protocol_rpc_client.h>

#include "polling_timeout_handler.h"

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_rpc.h>

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
  : m_any_functor{any_functor}
  , m_config{encoding}
{}

ProtocolRPCClient::ProtocolRPCClient(sup::dto::AnyFunctor& any_functor,
                                     ProtocolRPCClientConfig config)
  : m_any_functor{any_functor}
  , m_config{config}
{}

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
  catch(...)
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
  catch(...)
  {
    return ClientTransportException;
  }
  return HandleSyncInvokeReply(reply, output);
}

ProtocolResult ProtocolRPCClient::HandleAsyncInvoke(const sup::dto::AnyValue& input,
                                                    sup::dto::AnyValue& output)
{
  ProtocolResult result = Success;
  try
  {
    auto initial_reply = AsyncSendRequest(input);
    auto& sync_reply = std::get<2>(initial_reply);
    // Only if the reply was not asynchronous will this be non-empty:
    if (!sup::dto::IsEmptyValue(sync_reply))
    {
      return HandleSyncInvokeReply(sync_reply, output);
    }
    auto initial_result = std::get<1>(initial_reply);
    if (initial_result != Success)
    {
      return initial_result;
    }
    auto id = std::get<0>(initial_reply);
    auto poll_result = AsyncPoll(id);
    if (!poll_result.first)
    {
      return poll_result.second;
    }
    auto reply_info = AsyncGetReply(id);
    if (reply_info.first == Success && !sup::dto::IsEmptyValue(reply_info.second))
    {
      if (!sup::dto::TryAssignIfEmptyOrConvert(output, reply_info.second))
      {
        return ClientTransportDecodingError;
      }
    }
    result = reply_info.first;
  }
  catch(...)
  {
    return ClientTransportException;
  }
  return result;
}

std::tuple<sup::dto::uint64, ProtocolResult, sup::dto::AnyValue>
ProtocolRPCClient::AsyncSendRequest(const sup::dto::AnyValue& input)
{
  const std::tuple<sup::dto::uint64, ProtocolResult, sup::dto::AnyValue>
    failure{ 0, ClientTransportDecodingError, {} };
  const auto new_request = utils::CreateAsyncRPCRequest(input, m_config.m_encoding);
  auto initial_reply = m_any_functor(new_request);
  auto async_info = utils::GetAsyncInfo(initial_reply);
  if (!async_info.first)
  {
    // Provide the whole synchronous reply to be handled by the caller:
    return { 0, Success, initial_reply };
  }
  auto encoding_info = utils::TryGetPacketEncoding(initial_reply);
  if (!encoding_info.first)
  {
    return failure;
  }
  auto encoding = encoding_info.second;
  auto id_info = utils::TryExtractReplyId(initial_reply, encoding);
  if (!id_info.first || id_info.second == 0)
  {
    return failure;
  }
  return { id_info.second, Success, {} };
}

std::pair<bool, ProtocolResult> ProtocolRPCClient::AsyncPoll(sup::dto::uint64 id)
{
  const auto poll_request = utils::CreateAsyncRPCPoll(id, m_config.m_encoding);
  PollingTimeoutHandler polling_handler{m_config.m_timeout_sec, m_config.m_polling_interval_sec};
  while (true)
  {
    auto poll_reply = m_any_functor(poll_request);
    auto encoding_info = utils::TryGetPacketEncoding(poll_reply);
    if (!encoding_info.first)
    {
      return { false, ClientTransportDecodingError };
    }
    auto encoding = encoding_info.second;
    auto ready_info = utils::TryExtractReadyStatus(poll_reply, encoding);
    if (!ready_info.first)
    {
      return { false, ClientTransportDecodingError };
    }
    if (ready_info.second)
    {
      return { true, Success };
    }
    if (!polling_handler.Wait())
    {
      break;
    }
  }
  return { false, AsynchronousProtocolTimeout };
}

std::pair<ProtocolResult, sup::dto::AnyValue> ProtocolRPCClient::AsyncGetReply(sup::dto::uint64 id)
{
  const std::pair<ProtocolResult, sup::dto::AnyValue> failure{ ClientTransportDecodingError, {} };
  const auto get_reply_request = utils::CreateAsyncRPCGetReply(id, m_config.m_encoding);
  auto reply = m_any_functor(get_reply_request);
  auto result_info = utils::TryExtractProtocolResult(reply);
  if (!result_info.first)
  {
    return failure;
  }
  auto result = result_info.second;
  if (result != Success)
  {
    return { result, {} };
  }
  auto payload_info = TryGetPayload(reply);
  if (!payload_info.first)
  {
    return failure;
  }
  const auto& payload = payload_info.second;
  return { Success, payload };
}

namespace
{

std::pair<bool, sup::dto::AnyValue> TryGetPayload(const sup::dto::AnyValue& reply)
{
  std::pair<bool, sup::dto::AnyValue> failure{ false, {} };
  if (!reply.HasField(constants::REPLY_PAYLOAD))
  {
    return { true, {} };
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
  return { true, payload_result.second };
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
