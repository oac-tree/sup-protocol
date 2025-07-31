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

#ifndef SUP_PROTOCOL_LOG_PROTOCOL_DECORATOR_H_
#define SUP_PROTOCOL_LOG_PROTOCOL_DECORATOR_H_

#include <sup/protocol/protocol.h>

#include <functional>

namespace sup
{
namespace dto
{
class AnyValue;
}  // namespace dto

namespace protocol
{
class LogProtocolDecorator : public Protocol
{
public:
  enum class PacketType
  {
    kNormal = 0,
    kService
  };
  using LogInputFunction = std::function<void(const sup::dto::AnyValue&, PacketType)>;
  using LogOutputFunction = std::function<void(ProtocolResult, const sup::dto::AnyValue&, PacketType)>;
  LogProtocolDecorator(Protocol& protocol, LogInputFunction log_input_function,
                       LogOutputFunction log_output_function);
  ~LogProtocolDecorator() override;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;
  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  Protocol& m_protocol;
  LogInputFunction m_log_input_function;
  LogOutputFunction m_log_output_function;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_LOG_PROTOCOL_DECORATOR_H_
