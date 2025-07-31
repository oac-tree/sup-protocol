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
 * Copyright (c) : 2010-2025 ITER Organization,
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

#include <sup/protocol/log_protocol_decorator.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{

LogProtocolDecorator::LogProtocolDecorator(Protocol& protocol, LogFunction log_function)
  : m_protocol{protocol}
  , m_log_function{log_function}
{}

LogProtocolDecorator::~LogProtocolDecorator() = default;

ProtocolResult LogProtocolDecorator::Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  m_log_function(input, kLogProtocolRequestTitle);
  auto result = m_protocol.Invoke(input, output);
  m_log_function(output, kLogProtocolReplyTitle);
  return result;
}

ProtocolResult LogProtocolDecorator::Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  m_log_function(input, kLogProtocolServiceRequestTitle);
  auto result = m_protocol.Service(input, output);
  m_log_function(output, kLogProtocolServiceReplyTitle);
  return result;
}

}  // namespace protocol

}  // namespace sup
