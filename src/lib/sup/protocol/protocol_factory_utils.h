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
#include <sup/protocol/protocol_rpc.h>

#include <functional>
#include <memory>

namespace sup
{
namespace dto
{
class AnyFunctor;
}  // namespace dto

namespace protocol
{

/**
 * @brief Factory function that creates a server stack consisting of a ProtocolRPCServer
 * and a network server (created by the provided function and requiring the injection of a
 * ProtocolRPCServer with an injected Protocol).
 *
 * @param factory_func Factory function for the network server taking a sup::dto::AnyFunctor.
 * @param protocol Protocol to be injected into the encapsulated ProtocolRPCServer.
 *
 * @return An RPCServerInterface implementation.
 */
std::unique_ptr<RPCServerInterface> CreateRPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol);

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
