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
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "protocol_rpc.h"

#include <sup/protocol/protocol.h>
#include <sup/protocol/exceptions.h>

#include <sup/dto/anyvalue_helper.h>

#include <chrono>

namespace
{
sup::dto::AnyValue CreateApplicationProtocolRequestPayload();

sup::dto::AnyValue CreateApplicationProtocolReplyPayload(const std::string& application_type,
                                                         const std::string& application_version);
}  // unnamed namespace

namespace sup
{
namespace protocol
{
namespace utils
{
bool CheckRequestFormat(const sup::dto::AnyValue& request)
{
  // Only check type of timestamp field when present
  if (request.HasField(constants::REQUEST_TIMESTAMP)
      && request[constants::REQUEST_TIMESTAMP].GetType() != sup::dto::UnsignedInteger64Type)
  {
    return false;
  }
  if (!request.HasField(constants::REQUEST_PAYLOAD))
  {
    return false;
  }
  return true;
}

bool CheckReplyFormat(const sup::dto::AnyValue& reply)
{
  if (!reply.HasField(constants::REPLY_RESULT)
      || reply[constants::REPLY_RESULT].GetType() != sup::dto::UnsignedInteger32Type)
  {
    return false;
  }
  // Only check type of timestamp field when present
  if (reply.HasField(constants::REPLY_TIMESTAMP)
      && reply[constants::REPLY_TIMESTAMP].GetType() != sup::dto::UnsignedInteger64Type)
  {
    return false;
  }
  // Only check type of reason field when present
  if (reply.HasField(constants::REPLY_REASON)
      && reply[constants::REPLY_REASON].GetType() != sup::dto::StringType)
  {
    return false;
  }
  return true;
}

sup::dto::AnyValue CreateRPCRequest(const sup::dto::AnyValue& payload,
                                    PayloadEncoding encoding)
{
  if (sup::dto::IsEmptyValue(payload))
  {
    std::string error_message = "CreateRPCRequest(): empty payload is not allowed";
    throw InvalidOperationException(error_message);
  }
  sup::dto::AnyValue request = {{
    { constants::REQUEST_PAYLOAD, payload }
  }, constants::REQUEST_TYPE_NAME};
  return request;
}

sup::dto::AnyValue CreateRPCReply(const sup::protocol::ProtocolResult& result,
                                  const sup::dto::AnyValue& payload,
                                  PayloadEncoding encoding)
{
  sup::dto::AnyValue reply = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, result.GetValue()} }
  }, constants::REPLY_TYPE_NAME};
  if (!sup::dto::IsEmptyValue(payload))
  {
    reply.AddMember(constants::REPLY_PAYLOAD, payload);
  }
  return reply;
}

bool IsServiceRequest(const sup::dto::AnyValue& request)
{
  return request.HasField(constants::SERVICE_REQUEST_PAYLOAD);
}

bool CheckServiceReplyFormat(const sup::dto::AnyValue& reply)
{
  if (!reply.HasField(constants::SERVICE_REPLY_RESULT) ||
      reply[constants::SERVICE_REPLY_RESULT].GetType() != sup::dto::UnsignedInteger32Type)
  {
    return false;
  }
  return true;
}

sup::dto::AnyValue CreateServiceRequest(const sup::dto::AnyValue& payload,
                                        PayloadEncoding encoding)
{
  if (sup::dto::IsEmptyValue(payload))
  {
    std::string error_message = "CreateServiceRequest(): empty payload is not allowed";
    throw InvalidOperationException(error_message);
  }
  sup::dto::AnyValue service_request = {{
    { constants::SERVICE_REQUEST_PAYLOAD, payload }
  }, constants::SERVICE_REQUEST_TYPE_NAME};
  return service_request;
}

sup::dto::AnyValue CreateServiceReply(const sup::protocol::ProtocolResult& result,
                                      const sup::dto::AnyValue& payload,
                                      PayloadEncoding encoding)
{
  sup::dto::AnyValue service_reply = {{
    { constants::SERVICE_REPLY_RESULT, {sup::dto::UnsignedInteger32Type, result.GetValue()} }
  }, constants::SERVICE_REPLY_TYPE_NAME};
  if (!sup::dto::IsEmptyValue(payload))
  {
    service_reply.AddMember(constants::SERVICE_REPLY_PAYLOAD, payload);
  }
  return service_reply;
}

bool IsApplicationProtocolRequestPayload(const sup::dto::AnyValue& payload)
{
  return payload ==
    sup::dto::AnyValue{sup::dto::StringType, constants::APPLICATION_PROTOCOL_INFO_REQUEST};
}

bool CheckApplicationProtocolReplyPayload(const sup::dto::AnyValue& payload)
{
  if (!payload.HasField(constants::APPLICATION_PROTOCOL_TYPE) ||
      payload[constants::APPLICATION_PROTOCOL_TYPE].GetType() != sup::dto::StringType)
  {
    return false;
  }
  if (!payload.HasField(constants::APPLICATION_PROTOCOL_VERSION) ||
      payload[constants::APPLICATION_PROTOCOL_VERSION].GetType() != sup::dto::StringType)
  {
    return false;
  }
  return true;
}

ApplicationProtocolInfo GetApplicationProtocolInfo(Protocol& protocol)
{
  sup::dto::AnyValue reply;
  auto result = protocol.Service(CreateApplicationProtocolRequestPayload(), reply);
  if (result != sup::protocol::Success || !CheckApplicationProtocolReplyPayload(reply))
  {
    return {};
  }
  auto application_type = reply[constants::APPLICATION_PROTOCOL_TYPE].As<std::string>();
  auto application_version = reply[constants::APPLICATION_PROTOCOL_VERSION].As<std::string>();
  return { application_type, application_version };
}

sup::protocol::ProtocolResult HandleApplicationProtocolInfo(sup::dto::AnyValue& output,
                                                       const std::string& application_type,
                                                       const std::string& application_version)
{
  auto payload = CreateApplicationProtocolReplyPayload(application_type, application_version);
  if (!sup::dto::TryAssignIfEmptyOrConvert(output, payload))
  {
    return sup::protocol::ServerProtocolEncodingError;
  }
  return sup::protocol::Success;
}

}  // namespace utils

}  // namespace protocol

}  // namespace sup

namespace
{
using namespace sup::protocol;

sup::dto::AnyValue CreateApplicationProtocolRequestPayload()
{
  return {sup::dto::StringType, constants::APPLICATION_PROTOCOL_INFO_REQUEST};
}

sup::dto::AnyValue CreateApplicationProtocolReplyPayload(const std::string& application_type,
                                                         const std::string& application_version)
{
  sup::dto::AnyValue payload = {
    { constants::APPLICATION_PROTOCOL_TYPE, {sup::dto::StringType, application_type} },
    { constants::APPLICATION_PROTOCOL_VERSION, {sup::dto::StringType, application_version} }
  };
  return payload;
}
}  // unnamed namespace
