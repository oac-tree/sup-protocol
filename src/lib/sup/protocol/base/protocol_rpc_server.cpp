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

#include <sup/protocol/protocol_rpc_server.h>
#include <sup/protocol/exceptions.h>

#include <sup/protocol/base/async_invoke_server.h>

#include <sup/dto/anyvalue_helper.h>

namespace sup
{
namespace protocol
{

ProtocolRPCServer::ProtocolRPCServer(Protocol& protocol)
  : m_protocol{protocol}
  , m_async_server{new AsyncInvokeServer{m_protocol}}
{}

ProtocolRPCServer::~ProtocolRPCServer() = default;

sup::dto::AnyValue ProtocolRPCServer::operator()(const sup::dto::AnyValue& request)
{
  auto encoding_result = utils::TryGetPacketEncoding(request);
  if (!encoding_result.first)
  {
    return utils::CreateRPCReply(ServerUnsupportedPayloadEncodingError);
  }
  auto encoding = encoding_result.second;
  if (utils::IsServiceRequest(request))
  {
    return HandleServiceRequest(request, encoding);
  }
  return HandleInvokeRequest(request, encoding);
}

sup::dto::AnyValue ProtocolRPCServer::HandleInvokeRequest(const sup::dto::AnyValue& request,
                                                          PayloadEncoding encoding)
{
  if (!utils::CheckRequestFormat(request))
  {
    return utils::CreateRPCReply(ServerTransportDecodingError);
  }
  auto payload_result = utils::TryExtractRPCPayload(request, constants::REQUEST_PAYLOAD, encoding);
  if (!payload_result.first)
  {
    return utils::CreateRPCReply(ServerTransportDecodingError);
  }
  auto payload = payload_result.second;
  auto async_info = utils::GetAsyncInfo(request);
  if (async_info.first)
  {
    return m_async_server->HandleInvoke(payload, encoding, async_info.second);
  }
  sup::dto::AnyValue output;
  ProtocolResult result = Success;
  try
  {
    result = m_protocol.Invoke(payload, output);
  }
  catch(...)
  {
    return utils::CreateRPCReply(ServerProtocolException);
  }
  return utils::CreateRPCReply(result, output, encoding);
}

sup::dto::AnyValue ProtocolRPCServer::HandleServiceRequest(const sup::dto::AnyValue& request,
                                                           PayloadEncoding encoding)
{
  auto payload_result = utils::TryExtractRPCPayload(request, constants::SERVICE_REQUEST_PAYLOAD,
                                                    encoding);
  if (!payload_result.first)
  {
    return utils::CreateRPCReply(ServerTransportDecodingError);
  }
  auto payload = payload_result.second;
  sup::dto::AnyValue output;
  ProtocolResult result = Success;
  try
  {
    result = m_protocol.Service(payload, output);
  }
  catch(...)
  {
    return utils::CreateServiceReply(ServerProtocolException);
  }
  return utils::CreateServiceReply(result, output, encoding);
}

}  // namespace protocol

}  // namespace sup
