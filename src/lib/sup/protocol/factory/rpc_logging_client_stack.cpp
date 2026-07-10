/******************************************************************************
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include <sup/protocol/factory/rpc_logging_client_stack.h>

#include <sup/templates/decorate_with.h>

namespace sup
{
namespace protocol
{
namespace
{
std::unique_ptr<Protocol> DecorateProtocolWithLogger(
  std::unique_ptr<Protocol> protocol, const LoggingFunctions& log_functions);

std::unique_ptr<sup::dto::AnyFunctor> DecorateFunctorWithLogger(
  std::unique_ptr<sup::dto::AnyFunctor> functor,
  const LogAnyFunctorDecorator::LogFunction& log_function);
}  // unnamed namespace

RPCLoggingClientStack::RPCLoggingClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                               ProtocolRPCClientConfig config)
  : RPCLoggingClientStack{factory_func, config, {} }
{}

RPCLoggingClientStack::RPCLoggingClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                               ProtocolRPCClientConfig config,
                               const LoggingFunctions& log_functions)
  : m_rpc_client{DecorateFunctorWithLogger(factory_func(), log_functions.m_network_logger)}
  , m_protocol_client{DecorateProtocolWithLogger(
                        std::make_unique<ProtocolRPCClient>(*m_rpc_client, config),
                        log_functions)}
{}

RPCLoggingClientStack::~RPCLoggingClientStack() = default;

ProtocolResult RPCLoggingClientStack::Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  return m_protocol_client->Invoke(input, output);
}

ProtocolResult RPCLoggingClientStack::Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  return m_protocol_client->Service(input, output);
}

namespace
{
std::unique_ptr<Protocol> DecorateProtocolWithLogger(
  std::unique_ptr<Protocol> protocol, const LoggingFunctions& log_functions)
{
  if (log_functions.m_protocol_input_logger || log_functions.m_protocol_output_logger)
  {
    return sup::templates::DecorateWith<LogProtocolDecorator>(
      std::move(protocol), log_functions.m_protocol_input_logger,
      log_functions.m_protocol_output_logger);
  }
  return protocol;
}

std::unique_ptr<sup::dto::AnyFunctor> DecorateFunctorWithLogger(
  std::unique_ptr<sup::dto::AnyFunctor> functor,
  const LogAnyFunctorDecorator::LogFunction& log_function)
{
  if (log_function)
  {
    return sup::templates::DecorateWith<LogAnyFunctorDecorator>(
      std::move(functor), log_function);
  }
  return functor;
}
}  // unnamed namespace

}  // namespace protocol

}  // namespace sup
