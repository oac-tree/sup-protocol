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

LogProtocolDecorator::LogProtocolDecorator(Protocol& protocol, LogInputFunction log_input_function,
                                           LogOutputFunction log_output_function)
  : m_protocol{protocol}
  , m_log_input_function{log_input_function}
  , m_log_output_function{log_output_function}
{}

LogProtocolDecorator::~LogProtocolDecorator() = default;

ProtocolResult LogProtocolDecorator::Invoke(const sup::dto::AnyValue& input,
                                            sup::dto::AnyValue& output)
{
  m_log_input_function(input, PacketType::kNormal);
  auto result = m_protocol.Invoke(input, output);
  m_log_output_function(result, output, PacketType::kNormal);
  return result;
}

ProtocolResult LogProtocolDecorator::Service(const sup::dto::AnyValue& input,
                                             sup::dto::AnyValue& output)
{
  m_log_input_function(input, PacketType::kService);
  auto result = m_protocol.Service(input, output);
  m_log_output_function(result, output, PacketType::kService);
  return result;
}

}  // namespace protocol

}  // namespace sup
