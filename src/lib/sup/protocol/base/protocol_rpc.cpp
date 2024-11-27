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

#include <sup/protocol/protocol_rpc.h>

#include <sup/protocol/base/protocol_encodings.h>
#include <sup/protocol/protocol.h>
#include <sup/protocol/exceptions.h>

#include <sup/dto/anyvalue_helper.h>

#include <algorithm>
#include <chrono>

namespace
{
using namespace sup::protocol;
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
sup::dto::int32 EncodingToInteger(PayloadEncoding encoding)
{
  return static_cast<sup::dto::int32>(encoding);
}

PayloadEncoding EncodingFromInteger(sup::dto::int32 val)
{
  return static_cast<PayloadEncoding>(val);
}

bool IsSupportedPayloadEncoding(PayloadEncoding encoding)
{
  static const std::vector<PayloadEncoding> supported_encodings = {
    PayloadEncoding::kNone,
    PayloadEncoding::kBase64
  };
  auto it = std::find(supported_encodings.begin(), supported_encodings.end(), encoding);
  return it != supported_encodings.end();
}

bool CheckRequestFormat(const sup::dto::AnyValue& request)
{
  // Only check type of timestamp field when present
  if (request.HasField(constants::REQUEST_TIMESTAMP)
      && request[constants::REQUEST_TIMESTAMP].GetType() != sup::dto::UnsignedInteger64Type)
  {
    return false;
  }
  // Only check type of encoding field when present
  if (request.HasField(constants::ENCODING_FIELD_NAME)
      && request[constants::ENCODING_FIELD_NAME].GetType() != sup::dto::SignedInteger32Type)
  {
    return false;
  }
  // Only check type of async command field when present
  if (request.HasField(constants::ASYNC_COMMAND_FIELD_NAME)
      && request[constants::ASYNC_COMMAND_FIELD_NAME].GetType() != sup::dto::UnsignedInteger32Type)
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
  // Only check type of encoding field when present
  if (reply.HasField(constants::ENCODING_FIELD_NAME)
      && reply[constants::ENCODING_FIELD_NAME].GetType() != sup::dto::SignedInteger32Type)
  {
    return false;
  }
  // Only check type of reason field when present
  if (reply.HasField(constants::REPLY_REASON)
      && reply[constants::REPLY_REASON].GetType() != sup::dto::StringType)
  {
    return false;
  }
  // Only check type of async command field when present
  if (reply.HasField(constants::ASYNC_COMMAND_FIELD_NAME)
      && reply[constants::ASYNC_COMMAND_FIELD_NAME].GetType() != sup::dto::UnsignedInteger32Type)
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
  sup::dto::AnyValue request = sup::dto::EmptyStruct(constants::REQUEST_TYPE_NAME);
  AddRPCPayload(request, payload, constants::REQUEST_PAYLOAD, encoding);
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
    AddRPCPayload(reply, payload, constants::REPLY_PAYLOAD, encoding);
  }
  return reply;
}

sup::dto::AnyValue CreateAsyncRPCReply(const sup::protocol::ProtocolResult& result,
                                       const sup::dto::AnyValue& payload,
                                       PayloadEncoding encoding,
                                       AsyncCommand command)
{
  auto reply = CreateRPCReply(result, payload, encoding);
  reply.AddMember(constants::ASYNC_COMMAND_FIELD_NAME,
                  sup::dto::AnyValue{ sup::dto::UnsignedInteger32Type,
                                      static_cast<sup::dto::uint32>(command) });
  return reply;
}

sup::dto::AnyValue CreateRPCReply(const sup::protocol::ProtocolResult& result)
{
  // With empty payload, the encoding has no effect, so we pass kNone
  return CreateRPCReply(result, {}, PayloadEncoding::kNone);
}

sup::dto::AnyValue CreateAsyncRPCReply(const sup::protocol::ProtocolResult& result,
                                       AsyncCommand command)
{
  return CreateAsyncRPCReply(result, {}, PayloadEncoding::kNone, command);
}

bool IsServiceRequest(const sup::dto::AnyValue& request)
{
  // Only check type of encoding field when present
  if (request.HasField(constants::ENCODING_FIELD_NAME)
      && request[constants::ENCODING_FIELD_NAME].GetType() != sup::dto::SignedInteger32Type)
  {
    return false;
  }
  return request.HasField(constants::SERVICE_REQUEST_PAYLOAD);
}

bool CheckServiceReplyFormat(const sup::dto::AnyValue& reply)
{
  if (!reply.HasField(constants::SERVICE_REPLY_RESULT) ||
      reply[constants::SERVICE_REPLY_RESULT].GetType() != sup::dto::UnsignedInteger32Type)
  {
    return false;
  }
  // Only check type of encoding field when present
  if (reply.HasField(constants::ENCODING_FIELD_NAME)
      && reply[constants::ENCODING_FIELD_NAME].GetType() != sup::dto::SignedInteger32Type)
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
  sup::dto::AnyValue service_request = sup::dto::EmptyStruct(constants::SERVICE_REQUEST_TYPE_NAME);
  AddRPCPayload(service_request, payload, constants::SERVICE_REQUEST_PAYLOAD, encoding);
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
    AddRPCPayload(service_reply, payload, constants::SERVICE_REPLY_PAYLOAD, encoding);
  }
  return service_reply;
}

sup::dto::AnyValue CreateServiceReply(const sup::protocol::ProtocolResult& result)
{
  // With empty payload, the encoding has no effect, so we pass kNone
  return CreateServiceReply(result, {}, PayloadEncoding::kNone);
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

void AddRPCPayload(sup::dto::AnyValue& packet, const sup::dto::AnyValue& payload,
                   const std::string& member_name, PayloadEncoding encoding)
{
  if (encoding == PayloadEncoding::kNone)
  {
    (void)packet.AddMember(member_name, payload);
    return;
  }
  auto encoded_payload = encoding::Encode(payload, encoding);
  sup::dto::AnyValue encoding_field = EncodingToInteger(encoding);
  (void)packet.AddMember(constants::ENCODING_FIELD_NAME, encoding_field);
  (void)packet.AddMember(member_name, encoded_payload);
}

std::pair<bool, sup::dto::AnyValue> TryExtractRPCPayload(const sup::dto::AnyValue& packet,
                                                         const std::string& member_name,
                                                         PayloadEncoding encoding)
{
  std::pair<bool, sup::dto::AnyValue> failure{ false, {} };
  if (!packet.HasField(member_name))
  {
    return failure;
  }
  sup::dto::AnyValue payload{};
  try
  {
    payload = encoding::Decode(packet[member_name], encoding);
  }
  catch(...)
  {
    return failure;
  }
  return { true, payload };
}

std::pair<bool, PayloadEncoding> TryGetPacketEncoding(const sup::dto::AnyValue& packet)
{
  std::pair<bool, PayloadEncoding> failure{ false, PayloadEncoding::kNone };
  if (!packet.HasField(constants::ENCODING_FIELD_NAME))
  {
    return { true, PayloadEncoding::kNone};
  }
  auto encoding_field = packet[constants::ENCODING_FIELD_NAME];
  if (encoding_field.GetType() != sup::dto::SignedInteger32Type)
  {
    return failure;
  }
  auto encoding = utils::EncodingFromInteger(encoding_field.As<sup::dto::int32>());
  if (!IsSupportedPayloadEncoding(encoding))
  {
    return failure;
  }
  return { true, encoding };
}

std::pair<bool, AsyncCommand> GetAsyncInfo(const sup::dto::AnyValue& packet)
{
  if (packet.HasField(constants::ASYNC_COMMAND_FIELD_NAME) &&
      packet[constants::ASYNC_COMMAND_FIELD_NAME].GetType() == sup::dto::UnsignedInteger32Type)
  {
    auto command_nr = packet[constants::ASYNC_COMMAND_FIELD_NAME].As<sup::dto::uint32>();
    auto command = static_cast<AsyncCommand>(command_nr);
    return { true, command };
  }
  return { false, AsyncCommand::kInitialRequest };
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
