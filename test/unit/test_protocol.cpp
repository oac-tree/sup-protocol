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

}  // namespace test

}  // namespace protocol

}  // namespace sup
