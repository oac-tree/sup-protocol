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

#ifndef SUP_PROTOCOL_LOGGING_RPC_CLIENT_STACK_H_
#define SUP_PROTOCOL_LOGGING_RPC_CLIENT_STACK_H_

#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_factory_utils.h>
#include <sup/protocol/protocol_rpc_client.h>

#include <functional>
#include <memory>

namespace sup
{
namespace protocol
{

/**
 * @brief RPCLoggingClientStack is a logging implementation of Protocol that encapsulates both
 * the client's network implementation and a ProtocolRPCClient.
 */
class RPCLoggingClientStack : public Protocol
{
public:
  RPCLoggingClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                 PayloadEncoding encoding);

  RPCLoggingClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                 ProtocolRPCClientConfig config);

  RPCLoggingClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                 ProtocolRPCClientConfig config, const LoggingFunctions& log_functions);

  ~RPCLoggingClientStack() override;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  std::unique_ptr<sup::dto::AnyFunctor> m_rpc_client;
  std::unique_ptr<sup::protocol::Protocol> m_protocol_client;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_LOGGING_RPC_CLIENT_STACK_H_
