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
std::pair<bool, ProtocolResult> PollUntilReady(sup::dto::AnyFunctor& functor, sup::dto::uint64 id,
                                               PayloadEncoding encoding);
}  // unnamed namespace

ProtocolRPCClient::ProtocolRPCClient(dto::AnyFunctor& any_functor, PayloadEncoding encoding)
  : m_any_functor{any_functor}
  , m_encoding{encoding}
{}

ProtocolRPCClient::~ProtocolRPCClient() = default;

ProtocolResult ProtocolRPCClient::Invoke(const sup::dto::AnyValue& input,
                                         sup::dto::AnyValue& output)
{
  if (sup::dto::IsEmptyValue(input))
  {
    return ClientTransportEncodingError;
  }
  auto request = utils::CreateRPCRequest(input, m_encoding);
  sup::dto::AnyValue reply;
  try
  {
    reply = m_any_functor(request);
  }
  catch(...)
  {
    // If the previous code threw an exception, the next check will detect a malformed reply.
  }
  if (!utils::CheckReplyFormat(reply))
  {
    return ClientTransportDecodingError;
  }
  if (reply.HasField(constants::REPLY_PAYLOAD))
  {
    // TODO: parse encoding field instead
    auto payload_result = utils::TryExtractRPCReplyPayload(reply, m_encoding);
    if (!payload_result.first)
    {
      return ClientTransportDecodingError;
    }
    auto payload = payload_result.second;
    if (!sup::dto::TryAssignIfEmptyOrConvert(output, payload))
    {
      return ClientTransportDecodingError;
    }
  }
  return ProtocolResult{reply[constants::REPLY_RESULT].As<sup::dto::uint32>()};
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
    // If the previous code threw an exception, the next check will detect a malformed reply.
  }
  if (!utils::CheckServiceReplyFormat(reply))
  {
    return ClientTransportDecodingError;
  }
  if (reply.HasField(constants::SERVICE_REPLY_PAYLOAD))
  {
    // TODO: parse encoding field instead
    auto payload_result = utils::TryExtractServiceReplyPayload(reply, m_encoding);
    if (!payload_result.first)
    {
      return ClientTransportDecodingError;
    }
    auto payload = payload_result.second;
    if (!sup::dto::TryAssignIfEmptyOrConvert(output, payload))
    {
      return ClientTransportDecodingError;
    }
  }
  return ProtocolResult{reply[constants::SERVICE_REPLY_RESULT].As<sup::dto::uint32>()};
}

ProtocolResult ProtocolRPCClient::HandleSyncInvoke(const sup::dto::AnyValue& input,
                                                   sup::dto::AnyValue& output)
{
  return Success;
}

ProtocolResult ProtocolRPCClient::HandleAsyncInvoke(const sup::dto::AnyValue& input,
                                                    sup::dto::AnyValue& output)
{
  const auto new_request = utils::CreateAsyncRPCRequest(input, m_encoding);
  try
  {
    auto initial_reply = m_any_functor(new_request);
    auto encoding_info = utils::TryGetPacketEncoding(initial_reply);
    if (!encoding_info.first)
    {
      return ClientTransportDecodingError;
    }
    auto encoding = encoding_info.second;
    auto id_info = utils::TryExtractReplyId(initial_reply, encoding);
    if (!id_info.first)
    {
      return ClientTransportDecodingError;
    }
    auto id = id_info.second;
    auto ready_status = PollUntilReady(m_any_functor, id, encoding);
    if (ready_status.first)
    {
      // TODO: GetReply and copy to output
    }
    else
    {
      if (ready_status.second == Success)
      {
        return AsynchronousProtocolTimeout;
      }
      return ready_status.second;
    }
  }
  catch(...)
  {
    return ClientTransportDecodingError;
  }

  return Success;
}

namespace
{
std::pair<bool, ProtocolResult> PollUntilReady(sup::dto::AnyFunctor& functor, sup::dto::uint64 id,
                                               PayloadEncoding encoding)
{
  const auto poll_request = utils::CreateAsyncRPCPoll(id, encoding);
  while (true)
  {
    auto poll_reply = functor(poll_request);
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
  return { false, Success };  // protocol was successfull, but timeout expired
}

}  // unnamed namespace

}  // namespace protocol

}  // namespace sup
