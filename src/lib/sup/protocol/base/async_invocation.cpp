/******************************************************************************
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Marcelo Mariano (External)
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

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/async_invocation.h>

namespace sup
{
namespace protocol
{
namespace
{
// Extract the (optional) payload from a standard RPC reply packet.
std::pair<bool, sup::dto::AnyValue> TryGetReplyPayload(const sup::dto::AnyValue& reply)
{
  const std::pair<bool, sup::dto::AnyValue> failure{false, {}};
  if (!reply.HasField(constants::REPLY_PAYLOAD))
  {
    return {true, {}};
  }
  auto encoding_info = utils::TryGetPacketEncoding(reply);
  if (!encoding_info.first)
  {
    return failure;
  }
  auto payload_result = utils::TryExtractRPCReplyPayload(reply, encoding_info.second);
  if (!payload_result.first)
  {
    return failure;
  }
  return {true, payload_result.second};
}

// Decode a standard (synchronous style) RPC reply into the output payload.
ProtocolResult DecodeReply(const sup::dto::AnyValue& reply, sup::dto::AnyValue& output)
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
    auto payload_info = TryGetReplyPayload(reply);
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

AsyncInvocation::AsyncInvocation(sup::dto::AnyFunctor& functor, PayloadEncoding encoding)
    : m_functor{functor}, m_encoding{encoding}, m_id{0}, m_synchronous{false}, m_sync_reply{}
{
}

AsyncInvocation::~AsyncInvocation() = default;

ProtocolResult AsyncInvocation::Start(const sup::dto::AnyValue& input)
{
  const auto request = utils::CreateAsyncRPCRequest(input, m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_functor(request);
  }
  catch (...)
  {
    return ClientTransportException;
  }
  auto async_info = utils::GetAsyncInfo(reply);
  if (!async_info.first)
  {
    // Server replied synchronously: only accept a structurally valid reply and keep it for
    // GetReply(). This ensures IsSynchronous() never coexists with a malformed/empty reply.
    if (!utils::CheckReplyFormat(reply))
    {
      return ClientTransportDecodingError;
    }
    m_synchronous = true;
    m_sync_reply = reply;
    return Success;
  }
  auto encoding_info = utils::TryGetPacketEncoding(reply);
  if (!encoding_info.first)
  {
    return ClientTransportDecodingError;
  }
  auto id_info = utils::TryExtractReplyId(reply, encoding_info.second);
  if (!id_info.first || id_info.second == 0)
  {
    return ClientTransportDecodingError;
  }
  m_id = id_info.second;
  return Success;
}

std::pair<ProtocolResult, bool> AsyncInvocation::PollOnce()
{
  if (m_synchronous)
  {
    return {Success, true};
  }
  const auto poll_request = utils::CreateAsyncRPCPoll(m_id, m_encoding);
  sup::dto::AnyValue poll_reply;
  try
  {
    poll_reply = m_functor(poll_request);
  }
  catch (...)
  {
    return {ClientTransportException, false};
  }
  auto encoding_info = utils::TryGetPacketEncoding(poll_reply);
  if (!encoding_info.first)
  {
    return {ClientTransportDecodingError, false};
  }
  auto ready_info = utils::TryExtractReadyStatus(poll_reply, encoding_info.second);
  if (!ready_info.first)
  {
    return {ClientTransportDecodingError, false};
  }
  return {Success, ready_info.second};
}

ProtocolResult AsyncInvocation::GetReply(sup::dto::AnyValue& output)
{
  if (m_synchronous)
  {
    return DecodeReply(m_sync_reply, output);
  }
  const auto get_reply_request = utils::CreateAsyncRPCGetReply(m_id, m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_functor(get_reply_request);
  }
  catch (...)
  {
    return ClientTransportException;
  }
  auto result_info = utils::TryExtractProtocolResult(reply);
  if (!result_info.first)
  {
    return ClientTransportDecodingError;
  }
  auto result = result_info.second;
  if (result != Success)
  {
    return result;
  }
  auto payload_info = TryGetReplyPayload(reply);
  if (!payload_info.first)
  {
    return ClientTransportDecodingError;
  }
  const auto& payload = payload_info.second;
  if (!sup::dto::IsEmptyValue(payload) && !sup::dto::TryAssignIfEmptyOrConvert(output, payload))
  {
    return ClientTransportDecodingError;
  }
  return Success;
}

void AsyncInvocation::Invalidate()
{
  if (m_synchronous || m_id == 0)
  {
    return;
  }
  const auto invalidate_request = utils::CreateAsyncRPCInvalidate(m_id, m_encoding);
  try
  {
    (void)m_functor(invalidate_request);
  }
  catch (...)
  {
    // Best effort: ignore any failure when notifying the server.
  }
}

bool AsyncInvocation::IsSynchronous() const
{
  return m_synchronous;
}

}  // namespace protocol

}  // namespace sup
