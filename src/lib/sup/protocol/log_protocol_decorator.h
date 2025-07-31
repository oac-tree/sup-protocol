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
const std::string kLogProtocolRequestTitle = "Request protocol packet";
const std::string kLogProtocolReplyTitle = "Reply protocol packet";

const std::string kLogProtocolServiceRequestTitle = "Request protocol service packet";
const std::string kLogProtocolServiceReplyTitle = "Reply protocol service packet";

class LogProtocolDecorator : public Protocol
{
public:
  using LogFunction = std::function<void(const sup::dto::AnyValue&, const std::string&)>;
  LogProtocolDecorator(Protocol& protocol, LogFunction log_function);
  ~LogProtocolDecorator() override;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;
  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  Protocol& m_protocol;
  LogFunction m_log_function;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_LOG_PROTOCOL_DECORATOR_H_
