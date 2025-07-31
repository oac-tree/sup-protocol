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

#ifndef SUP_PROTOCOL_LOG_ANYVALUE_FUNCTOR_DECORATOR_H_
#define SUP_PROTOCOL_LOG_ANYVALUE_FUNCTOR_DECORATOR_H_

#include <sup/dto/any_functor.h>

#include <functional>

namespace sup
{
namespace dto
{
class AnyValue;
}  // namespace dto

namespace protocol
{
const std::string kLogRequestTitle = "Request network packet";
const std::string kLogReplyTitle = "Reply network packet";

class LogAnyValueFunctorDecorator : public sup::dto::AnyFunctor
{
public:
  using LogFunction = std::function<void(const sup::dto::AnyValue&, const std::string&)>;
  LogAnyValueFunctorDecorator(sup::dto::AnyFunctor& functor, LogFunction log_function);
  ~LogAnyValueFunctorDecorator() override;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

private:
  sup::dto::AnyFunctor& m_functor;
  LogFunction m_log_function;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_H_
