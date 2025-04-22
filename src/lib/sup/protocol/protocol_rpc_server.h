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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_SERVER_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_SERVER_H_

#include <sup/protocol/protocol_rpc_server_config.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/protocol.h>

#include <sup/dto/any_functor.h>
#include <sup/dto/basic_scalar_types.h>

namespace sup
{
namespace protocol
{
class AsyncInvokeServer;
class ExpirationTimeoutHandler;

/**
 * @brief The ProtocolRPCServer is an AnyFunctor implementation that forwards to a Protocol.
 *
 * @details The ProtocolRPCServer is intended to be used as a mediator between an RPC server
 * and a Protocol implementation.
 *
 * The input structure needs to conform to an RPC request structure and it returns an RPC reply
 * structure (see protocol_rpc.h).
 */
class ProtocolRPCServer : public sup::dto::AnyFunctor
{
public:
  explicit ProtocolRPCServer(Protocol& protocol);
  ProtocolRPCServer(Protocol& protocol, ProtocolRPCServerConfig config);
  ~ProtocolRPCServer() override;

  ProtocolRPCServer(const ProtocolRPCServer&) = default;
  ProtocolRPCServer(ProtocolRPCServer&&) = delete;
  ProtocolRPCServer& operator=(const ProtocolRPCServer&) = delete;
  ProtocolRPCServer& operator=(ProtocolRPCServer&&) = delete;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;
private:
  sup::dto::AnyValue HandleInvokeRequest(const sup::dto::AnyValue& request,
                                         PayloadEncoding encoding);
  sup::dto::AnyValue HandleServiceRequest(const sup::dto::AnyValue& request,
                                          PayloadEncoding encoding);
  Protocol& m_protocol;
  std::unique_ptr<AsyncInvokeServer> m_async_server;
  std::unique_ptr<ExpirationTimeoutHandler> m_expiration_handler;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_SERVER_H_
