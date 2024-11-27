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

#include "test_protocol.h"

#include <sup/protocol/protocol_rpc.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <cmath>
#include <stdexcept>

namespace sup
{
namespace protocol
{
namespace test
{
TestProtocol::TestProtocol()
  : m_last_input{}
  , m_service_fail{false}
  , m_service_throw{false}
{}

TestProtocol::~TestProtocol() = default;

ProtocolResult TestProtocol::Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  m_last_input.reset(new sup::dto::AnyValue(input));
  if (input.HasField(ECHO_FIELD) && input[ECHO_FIELD].As<bool>())
  {
    sup::dto::TryAssignIfEmptyOrConvert(output, input);
  }
  if (input.HasField(THROW_FIELD) && input[THROW_FIELD].As<bool>())
  {
    throw std::runtime_error("Throwing on demand");
  }
  if (input.HasField(REQUESTED_STATUS_FIELD) &&
      input[REQUESTED_STATUS_FIELD].GetType() == sup::dto::UnsignedInteger32Type)
  {
    return ProtocolResult(input[REQUESTED_STATUS_FIELD].As<sup::dto::uint32>());
  }
  return Success;
}

ProtocolResult TestProtocol::Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  m_last_input.reset(new sup::dto::AnyValue(input));
  if (m_service_fail)
  {
    return sup::protocol::ServerTransportEncodingError;
  }
  if (m_service_throw)
  {
    throw std::runtime_error("Throwing on demand");
  }
  if (utils::IsApplicationProtocolRequestPayload(input))
  {
    return utils::HandleApplicationProtocolInfo(output, TEST_PROTOCOL_TYPE, TEST_PROTOCOL_VERSION);
  }
  return Success;
}

void TestProtocol::SetFailForServiceRequest(bool fail)
{
  m_service_fail = fail;
}

void TestProtocol::SetThrowForServiceRequest(bool enable_throw)
{
  m_service_throw = enable_throw;
}

sup::dto::AnyValue TestProtocol::GetLastInput() const
{
  if (!m_last_input)
  {
    return {};
  }
  sup::dto::AnyValue result{*m_last_input};
  return result;
}

AsyncRequestTestProtocol::AsyncRequestTestProtocol(std::future<void> go_future)
  : m_go_future{std::move(go_future)}
{}

ProtocolResult AsyncRequestTestProtocol::Invoke(const sup::dto::AnyValue& input,
                                                sup::dto::AnyValue& output)
{
  output = input;
  m_go_future.get();
  return Success;
}

ProtocolResult AsyncRequestTestProtocol::Service(const sup::dto::AnyValue& input,
                                                 sup::dto::AnyValue& output)
{
  output = input;
  return Success;
}

sup::dto::uint64 ExtractRequestId(const sup::dto::AnyValue& reply)
{
  auto encoding_result = utils::TryGetPacketEncoding(reply);
  if (!encoding_result.first)
  {
    return 0;
  }
  auto encoding = encoding_result.second;
  auto payload_result = utils::TryExtractRPCPayload(reply, constants::REPLY_PAYLOAD, encoding);
  if (!payload_result.first)
  {
    return 0;
  }
  auto payload = payload_result.second;
  if (!payload.HasField(constants::ASYNC_ID_FIELD_NAME))
  {
    return 0;
  }
  auto& id_field = payload[constants::ASYNC_ID_FIELD_NAME];
  if (id_field.GetType() != sup::dto::UnsignedInteger64Type)
  {
    return 0;
  }
  return id_field.As<sup::dto::uint64>();
}

sup::dto::uint32 ExtractReadyStatus(const sup::dto::AnyValue& reply)
{
  auto encoding_result = utils::TryGetPacketEncoding(reply);
  if (!encoding_result.first)
  {
    return 0;
  }
  auto encoding = encoding_result.second;
  auto payload_result = utils::TryExtractRPCPayload(reply, constants::REPLY_PAYLOAD, encoding);
  if (!payload_result.first)
  {
    return 0;
  }
  auto payload = payload_result.second;
  if (!payload.HasField(constants::ASYNC_READY_FIELD_NAME))
  {
    return 0;
  }
  auto& ready_field = payload[constants::ASYNC_READY_FIELD_NAME];
  if (ready_field.GetType() != sup::dto::UnsignedInteger32Type)
  {
    return 0;
  }
  return ready_field.As<sup::dto::uint32>();
}

bool PollUntilReady(sup::dto::AnyFunctor& functor, sup::dto::uint64 id, double seconds)
{
  auto poll_request = utils::CreateAsyncRPCPoll(id, PayloadEncoding::kBase64);
  auto timeout_ns = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  auto start = std::chrono::steady_clock::now();
  while (true)
  {
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - start);
    if (elapsed > timeout_ns)
    {
      return false;
    }
    auto reply = functor(poll_request);
    if (ExtractReadyStatus(reply) == 1u)
    {
      return true;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

}  // namespace test

}  // namespace protocol

}  // namespace sup
