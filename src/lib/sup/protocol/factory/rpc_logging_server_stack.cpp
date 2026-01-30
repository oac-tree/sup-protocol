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

#include <sup/protocol/factory/rpc_logging_server_stack.h>

#include <sup/protocol/log_any_functor_decorator.h>
#include <sup/protocol/log_protocol_decorator.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <sup/templates/decorate_with.h>

namespace sup
{
namespace protocol
{
namespace
{
std::unique_ptr<Protocol> DecorateProtocolWithLogger(
  std::unique_ptr<Protocol> protocol, const LoggingFunctions& log_functions);

std::unique_ptr<sup::dto::AnyFunctor> BuildFunctorFromProtocol(
  ProtocolRPCServerConfig config, Protocol& protocol,
  const LoggingFunctions& log_functions);

std::unique_ptr<sup::dto::AnyFunctor> DecorateFunctorWithLogger(
  std::unique_ptr<sup::dto::AnyFunctor> functor,
  const LogAnyFunctorDecorator::LogFunction& log_function);
}

RPCLoggingServerStack::RPCLoggingServerStack(
    std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
    ProtocolRPCServerConfig config, std::unique_ptr<Protocol> protocol)
  : RPCLoggingServerStack{factory_func, config, std::move(protocol), {} }
{}

RPCLoggingServerStack::RPCLoggingServerStack(
    std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
    ProtocolRPCServerConfig config, std::unique_ptr<Protocol> protocol,
    const LoggingFunctions& log_functions)
  : m_protocol{DecorateProtocolWithLogger(std::move(protocol), log_functions)}
  , m_functor{BuildFunctorFromProtocol(config, *m_protocol, log_functions)}
  , m_rpc_server{factory_func(*m_functor)}
{}

RPCLoggingServerStack::RPCLoggingServerStack(
    std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
    std::unique_ptr<sup::dto::AnyFunctor> functor,
    const LogAnyFunctorDecorator::LogFunction& log_function)
  : m_protocol{}
  , m_functor{DecorateFunctorWithLogger(std::move(functor), log_function)}
  , m_rpc_server{factory_func(*m_functor)}
{}

RPCLoggingServerStack::~RPCLoggingServerStack() = default;

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

std::unique_ptr<sup::dto::AnyFunctor> BuildFunctorFromProtocol(
  ProtocolRPCServerConfig config, Protocol& protocol,
  const LoggingFunctions& log_functions)
{
  std::unique_ptr<sup::dto::AnyFunctor> functor =
    std::make_unique<ProtocolRPCServer>(protocol, config);
  return DecorateFunctorWithLogger(std::move(functor), log_functions.m_network_logger);
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
}

}  // namespace protocol

}  // namespace sup
