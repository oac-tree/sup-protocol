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
 * Copyright (c) : 2010-2023 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "protocol_rpc_client.h"

#include <sup/dto/anyvalue_helper.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace protocol
{

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
    auto payload = utils::ExtractRPCPayload(reply, constants::REPLY_PAYLOAD);
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
    auto payload = utils::ExtractRPCPayload(reply, constants::SERVICE_REPLY_PAYLOAD);
    if (!sup::dto::TryAssignIfEmptyOrConvert(output, payload))
    {
      return ClientTransportDecodingError;
    }
  }
  return ProtocolResult{reply[constants::SERVICE_REPLY_RESULT].As<sup::dto::uint32>()};
}

}  // namespace protocol

}  // namespace sup
