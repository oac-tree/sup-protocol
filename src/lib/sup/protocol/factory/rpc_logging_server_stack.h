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

#ifndef SUP_PROTOCOL_RPC_LOGGING_SERVER_STACK_H_
#define SUP_PROTOCOL_RPC_LOGGING_SERVER_STACK_H_

#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_factory_utils.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <functional>
#include <memory>

namespace sup
{
namespace protocol
{

/**
 * @brief RPCLoggingServerStack is a logging implementation of RPCServerInterface
 * that encapsulates both the server's implementation, which requires injecting a ProtocolRPCServer,
 * and the ProtocolRPCServer.
 */
class RPCLoggingServerStack : public RPCServerInterface
{
public:
  RPCLoggingServerStack(
    std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
    ProtocolRPCServerConfig config, std::unique_ptr<Protocol> protocol,
    const LoggingFunctions& log_functions);

  ~RPCLoggingServerStack() override;

private:
  std::unique_ptr<Protocol> m_protocol;
  std::unique_ptr<sup::dto::AnyFunctor> m_functor;
  std::unique_ptr<RPCServerInterface> m_rpc_server;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_RPC_LOGGING_SERVER_STACK_H_
