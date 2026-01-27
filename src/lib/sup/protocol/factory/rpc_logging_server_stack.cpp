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

namespace sup
{
namespace protocol
{

RPCLoggingServerStack::RPCLoggingServerStack(
    std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
    ProtocolRPCServerConfig config, std::unique_ptr<Protocol> protocol,
    LogAnyFunctorDecorator::LogFunction log_function)
  : m_protocol{std::move(protocol)}
  , m_protocol_server{*m_protocol, config}
  , m_log_decorator{m_protocol_server, log_function}
  , m_rpc_server{factory_func(m_log_decorator)}
{}

RPCLoggingServerStack::~RPCLoggingServerStack() = default;

}  // namespace protocol

}  // namespace sup
