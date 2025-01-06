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
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_PROTOCOL_PROTOCOL_FACTORY_H_
#define SUP_PROTOCOL_PROTOCOL_FACTORY_H_

#include <sup/protocol/process_variable.h>
#include <sup/protocol/protocol.h>

#include <memory>

namespace sup
{
namespace protocol
{
/**
 * @brief RPCServerInterface is an empty interface for RPC servers. It does not define any methods,
 * since the server is assumed to be running when constructed until its destruction.
 */
class RPCServerInterface
{
public:
  RPCServerInterface() = default;
  RPCServerInterface(const RPCServerInterface&) = delete;
  RPCServerInterface(RPCServerInterface&&) = delete;
  RPCServerInterface& operator=(const RPCServerInterface&) = delete;
  RPCServerInterface& operator=(RPCServerInterface&&) = delete;
  virtual ~RPCServerInterface();
};

/**
 * @brief The ProtocolFactory is an interface for factories that can create RPC clients/servers
 * and network variables.
 *
 * @details The ProtocolFactory is intended to facilitate the dynamic creation of RPC
 * clients/servers and network variables. It can also provide mechanisms of automatically
 * decorating certain protocols.
 */
class ProtocolFactory
{
public:
  ProtocolFactory() = default;
  ProtocolFactory(const ProtocolFactory&) = delete;
  ProtocolFactory(ProtocolFactory&&) = delete;
  ProtocolFactory& operator=(const ProtocolFactory&) = delete;
  ProtocolFactory& operator=(ProtocolFactory&&) = delete;
  virtual ~ProtocolFactory();

  /**
   * @brief Creates a ProcessVariable.
   *
   * @param var_definition Definition of the ProcessVariable: may include the used network protocol,
   * the name to connect to the variable on the network, etc.
   *
   * @return ProcessVariable.
   * @throws InvalidOperationException when the factory cannot create a ProcessVariable with the
   * given definition.
   */
  virtual std::unique_ptr<ProcessVariable> CreateProcessVariable(
    const sup::dto::AnyValue& var_definition) const = 0;

  /**
   * @brief Creates an RPC server for the given Protocol and with the given server definition.
   *
   * @param protocol Protocol to inject into the server.
   * @param server_definition Definition of the server: may include the used network protocol,
   * server name, etc.
   *
   * @return RPC server.
   * @throws InvalidOperationException when the factory cannot create a server with the given
   * definition.
   */
  virtual std::unique_ptr<RPCServerInterface> CreateRPCServer(Protocol& protocol,
    const sup::dto::AnyValue& server_definition) const = 0;

  /**
   * @brief Creates an RPC client with the given client definition.
   *
   * @param client_definition Definition of the client: may include the used network protocol,
   * server name to connect to, etc.
   *
   * @return Protocol encapsulating the RPC client.
   * @throws InvalidOperationException when the factory cannot create a client with the given
   * definition.
   */
  virtual std::unique_ptr<Protocol> CreateRPCClient(
    const sup::dto::AnyValue& client_definition) const = 0;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_FACTORY_H_
