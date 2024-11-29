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
 * Copyright (c) : 2010-2024 ITER Organization,
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
}  // unnamed namespace

ProtocolRPCClient::ProtocolRPCClient(dto::AnyFunctor& any_functor, PayloadEncoding encoding)
  : m_any_functor{any_functor}
  , m_encoding{encoding}
{}

ProtocolRPCClient::~ProtocolRPCClient() = default;

ProtocolResult ProtocolRPCClient::Invoke(const sup::dto::AnyValue& input,
                                         sup::dto::AnyValue& output)
{
  // TODO: this must become a config parameter:
  bool async = false;
  if (sup::dto::IsEmptyValue(input))
  {
    return ClientTransportEncodingError;
  }
  if (async)
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
  auto request = utils::CreateServiceRequest(input, m_encoding);
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
  auto request = utils::CreateRPCRequest(input, m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_any_functor(request);
  }
  catch(...)
  {
    return ClientTransportException;
  }
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

ProtocolResult ProtocolRPCClient::HandleAsyncInvoke(const sup::dto::AnyValue& input,
                                                    sup::dto::AnyValue& output)
{
  ProtocolResult result = Success;
  try
  {
    auto initial_reply = AsyncSendRequest(input);
    auto initial_result = initial_reply.second;
    if (initial_result != Success)
    {
      return initial_result;
    }
    auto id = initial_reply.first;
    auto poll_result = AsyncPoll(id);
    if (!poll_result.first)
    {
      return poll_result.second;
    }
    auto reply_info = AsynGetReply(id);
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

std::pair<sup::dto::uint64, ProtocolResult> ProtocolRPCClient::AsyncSendRequest(
  const sup::dto::AnyValue& input)
{
  const std::pair<sup::dto::uint64, ProtocolResult> failure{ 0, ClientTransportDecodingError };
  const auto new_request = utils::CreateAsyncRPCRequest(input, m_encoding);
  auto initial_reply = m_any_functor(new_request);
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
  return { id_info.second, Success };
}

std::pair<bool, ProtocolResult> ProtocolRPCClient::AsyncPoll(sup::dto::uint64 id)
{
  const auto poll_request = utils::CreateAsyncRPCPoll(id, m_encoding);
  while (true)
  {
    // TODO: catch exceptions
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
    // TODO: sleep and exit if timeout expired
  }
  return { false, AsynchronousProtocolTimeout };
}

std::pair<ProtocolResult, sup::dto::AnyValue> ProtocolRPCClient::AsynGetReply(sup::dto::uint64 id)
{
  const std::pair<ProtocolResult, sup::dto::AnyValue> failure{ ClientTransportDecodingError, {} };
  const auto get_reply_request = utils::CreateAsyncRPCGetReply(id, m_encoding);
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

}  // unnamed namespace

}  // namespace protocol

}  // namespace sup
