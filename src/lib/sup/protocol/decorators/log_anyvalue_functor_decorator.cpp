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

#include <sup/protocol/log_anyvalue_functor_decorator.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{

LogAnyValueFunctorDecorator::LogAnyValueFunctorDecorator(
  sup::dto::AnyFunctor& functor, LogFunction log_function)
  : m_functor{functor}
  , m_log_function{log_function}
{}

LogAnyValueFunctorDecorator::~LogAnyValueFunctorDecorator() = default;

sup::dto::AnyValue LogAnyValueFunctorDecorator::operator()(const sup::dto::AnyValue& input)
{
  m_log_function(input, kLogRequestTitle);
  auto reply = m_functor(input);
  m_log_function(reply, kLogReplyTitle);
  return reply;
}

}  // namespace protocol

}  // namespace sup
