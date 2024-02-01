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
 * Copyright (c) : 2010-2023 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_
#define SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_

#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_rpc_client.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <functional>
#include <memory>

namespace sup
{
namespace protocol
{

/**
 * @brief RPCServerStack is an RPCServerInterface that encapsulates both the server's
 * implementation, which requires injecting a ProtocolRPCServer, and the ProtocolRPCServer.
 */
class RPCServerStack : public RPCServerInterface
{
public:
  RPCServerStack(std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
                 Protocol& protocol);

  ~RPCServerStack();

private:
  ProtocolRPCServer m_protocol_server;
  std::unique_ptr<RPCServerInterface> m_rpc_server;
};

/**
 * @brief Factory function that creates a server stack consisting of a network server (created
 * by the provided function and requiring the injection of a ProtocolRPCServer with an injected
 * Protocol).
 *
 * @param factory_func Factory function for the network server taking a sup::dto::AnyFunctor.
 * @param protocol Protocol to be injected into the encapsulated ProtocolRPCServer.
 *
 * @return A RPCServerInterface implementation.
 */
std::unique_ptr<RPCServerInterface> CreateRPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol);

/**
 * @brief RPCClientStack is a Protocol that encapsulates both the client's network implementation
 * and a ProtocolRPCClient.
 */
class RPCClientStack : public Protocol
{
public:
  RPCClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                 PayloadEncoding encoding);

  ~RPCClientStack();

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  std::unique_ptr<sup::dto::AnyFunctor> m_rpc_client;
  ProtocolRPCClient m_protocol_client;
};

/**
 * @brief Factory function that creates a server stack consisting of a network server (created
 * by the provided function and requiring the injection of a ProtocolRPCServer with an injected
 * Protocol).
 *
 * @param factory_func Factory function for the network server taking a sup::dto::AnyFunctor.
 * @param protocol Protocol to be injected into the encapsulated ProtocolRPCServer.
 *
 * @return A RPCServerInterface implementation.
 */
std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func, PayloadEncoding encoding);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_
