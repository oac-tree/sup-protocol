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

#ifndef SUP_PROTOCOL_LOG_ANYFUNCTOR_DECORATOR_H_
#define SUP_PROTOCOL_LOG_ANYFUNCTOR_DECORATOR_H_

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
class LogAnyFunctorDecorator : public sup::dto::AnyFunctor
{
public:
  enum class PacketDirection
  {
    kInput = 0,
    kOutput
  };
  using LogFunction = std::function<void(const sup::dto::AnyValue&, PacketDirection)>;
  LogAnyFunctorDecorator(sup::dto::AnyFunctor& functor, LogFunction log_function);
  ~LogAnyFunctorDecorator() override;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

private:
  sup::dto::AnyFunctor& m_functor;
  LogFunction m_log_function;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_LOG_ANYFUNCTOR_DECORATOR_H_
