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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_H_

#include <sup/dto/anyvalue.h>
#include <sup/protocol/protocol_result.h>

#include <utility>

namespace sup
{
namespace protocol
{
class Protocol;

namespace constants
{
/**
 * Generic fields in transport packets:
 * - encoding: specifies how the payload is encoded
 * - async: specifies a command for asynchronous RPC calls
 * - id: provides the identification of a specific asynchronous RPC call
 * - ready: provides the readiness of the reply for a specific asynchronous RPC call
*/
const std::string ENCODING_FIELD_NAME = "encoding";
const std::string ASYNC_COMMAND_FIELD_NAME = "async";
const std::string ASYNC_ID_FIELD_NAME = "id";
const std::string ASYNC_READY_FIELD_NAME = "ready";

/**
 * An RPC request is a structured AnyValue with two fields:
 * - timestamp: a 64bit unsigned integer (obsolete)
 * - encoding: specifies how the payload is encoded (optional: default is no encoding)
 * - async: specifies a command for asynchronous RPC calls
 * - query: a generic AnyValue that contains the payload of the request
 * Obsolete fields need to have the correct type when present.
*/
const std::string REQUEST_TYPE_NAME = "sup::protocolRequest/v2.1";
const std::string REQUEST_TIMESTAMP = "timestamp";
const std::string REQUEST_PAYLOAD = "query";

/**
 * An RPC reply is a structured AnyValue with the following fields:
 * - result: a 32bit unsigned integer denoting success or failure status
 * - timestamp: a 64bit unsigned integer (obsolete)
 * - encoding: specifies how the payload is encoded
 * - async: specifies the reply type for asynchronous RPC calls
 * - reason: string (obsolete)
 * - reply (optional): a field that contains the payload of the RPC reply (may be omitted if not
 *                     required)
 * Obsolete fields need to have the correct type when present.
*/
const std::string REPLY_TYPE_NAME = "sup::protocolReply/v2.1";
const std::string REPLY_RESULT = "result";
const std::string REPLY_TIMESTAMP = "timestamp";
const std::string REPLY_REASON = "reason";
const std::string REPLY_PAYLOAD = "reply";

/**
 * A service request is a simple packet, used for querying the application protocol. It will
 * never reach the application layer and is intercepted by the application layer specific
 * protocol implementation. This allows providing generic information about the application
 * interface that is encapsulated by the protocol.
 * It contains:
 * - encoding: specifies how the payload is encoded
 * - service: a payload identifying the service request
 * Note that service requests do not support asynchronous calls, as they are assumed to be fast.
*/
const std::string SERVICE_REQUEST_TYPE_NAME = "sup::ServiceRequest/v2.0";
const std::string SERVICE_REQUEST_PAYLOAD = "service";

/**
 * A service reply is a structured AnyValue with the following fields:
 * - encoding: specifies how the payload is encoded
 * - result: a 32bit unsigned integer denoting success or failure status
 * - reply (optional): the payload of the service reply
*/
const std::string SERVICE_REPLY_TYPE_NAME = "sup::ServiceReply/v2.0";
const std::string SERVICE_REPLY_RESULT = "result";
const std::string SERVICE_REPLY_PAYLOAD = "reply";

/**
 * Currently only one generic service request and reply is defined: the request to receive the
 * name and version of the application protocol. Any other service request will be application
 * protocol specific and can be requested after identifying the application protocol.
 * The reply for this application protocol info contains:
 * - application_type: string identifying the encapsulated application interface
 * - application_version: string identifying the version for the application protocol
*/
const std::string APPLICATION_PROTOCOL_INFO_REQUEST = "application_protocol_info";
const std::string APPLICATION_PROTOCOL_TYPE = "application_type";
const std::string APPLICATION_PROTOCOL_VERSION = "application_version";

}  // namespace constants

enum class PayloadEncoding : sup::dto::int32
{
  kNone = 0,
  kBase64
};

enum class AsyncCommand : sup::dto::uint32
{
  kInitialRequest = 0u,
  kPoll,
  kGetReply,
  kInvalidate
};

namespace utils
{
sup::dto::int32 EncodingToInteger(PayloadEncoding encoding);

PayloadEncoding EncodingFromInteger(sup::dto::int32 val);

bool IsSupportedPayloadEncoding(PayloadEncoding encoding);

/**
 * Structure that identifies a specific application protocol with its version.
*/
struct ApplicationProtocolInfo
{
  std::string m_application_type;
  std::string m_application_version;
};

bool CheckRequestFormat(const sup::dto::AnyValue& request);

bool CheckReplyFormat(const sup::dto::AnyValue& reply);

sup::dto::AnyValue CreateRPCRequest(const sup::dto::AnyValue& payload, PayloadEncoding encoding);

sup::dto::AnyValue CreateAsyncRPCRequest(const sup::dto::AnyValue& payload,
                                         PayloadEncoding encoding);

sup::dto::AnyValue CreateAsyncRPCPoll(sup::dto::uint64 id, PayloadEncoding encoding);

sup::dto::AnyValue CreateAsyncRPCGetReply(sup::dto::uint64 id, PayloadEncoding encoding);

sup::dto::AnyValue CreateAsyncRPCInvalidate(sup::dto::uint64 id, PayloadEncoding encoding);

sup::dto::AnyValue CreateRPCReply(const sup::protocol::ProtocolResult& result,
                                  const sup::dto::AnyValue& payload,
                                  PayloadEncoding encoding);

sup::dto::AnyValue CreateAsyncRPCReply(const sup::protocol::ProtocolResult& result,
                                       const sup::dto::AnyValue& payload,
                                       PayloadEncoding encoding,
                                       AsyncCommand command);

sup::dto::AnyValue CreateRPCReply(const sup::protocol::ProtocolResult& result);

sup::dto::AnyValue CreateAsyncRPCReply(const sup::protocol::ProtocolResult& result,
                                       AsyncCommand command);

bool IsServiceRequest(const sup::dto::AnyValue& request);

bool CheckServiceReplyFormat(const sup::dto::AnyValue& reply);

sup::dto::AnyValue CreateServiceRequest(const sup::dto::AnyValue& payload,
                                        PayloadEncoding encoding);

sup::dto::AnyValue CreateServiceReply(const sup::protocol::ProtocolResult& result,
                                      const sup::dto::AnyValue& payload,
                                      PayloadEncoding encoding);

sup::dto::AnyValue CreateServiceReply(const sup::protocol::ProtocolResult& result);

bool IsApplicationProtocolRequestPayload(const sup::dto::AnyValue& payload);

bool CheckApplicationProtocolReplyPayload(const sup::dto::AnyValue& payload);

ApplicationProtocolInfo GetApplicationProtocolInfo(Protocol& protocol);

sup::protocol::ProtocolResult HandleApplicationProtocolInfo(
  sup::dto::AnyValue& output, const std::string& application_type,
  const std::string& application_version);

void AddRPCPayload(sup::dto::AnyValue& packet, const sup::dto::AnyValue& payload,
                   const std::string& member_name, PayloadEncoding encoding);

std::pair<bool, sup::dto::AnyValue> TryExtractRPCPayload(const sup::dto::AnyValue& packet,
                                                         const std::string& member_name,
                                                         PayloadEncoding encoding);

std::pair<bool, PayloadEncoding> TryGetPacketEncoding(const sup::dto::AnyValue& packet);

std::pair<bool, AsyncCommand> GetAsyncInfo(const sup::dto::AnyValue& packet);

}  // namespace utils

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_H_
