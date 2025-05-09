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

#ifndef SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_
#define SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_

#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_factory.h>
#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/protocol_rpc_client_config.h>

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
const std::string kEncoding = "Encoding";
const std::string kEncoding_None = "None";
const std::string kEncoding_Base64 = "Base64";

/**
 * @brief Factory function that creates a server stack consisting of a ProtocolRPCServer
 * and a network server (created by the provided function and requiring the injection of a
 * ProtocolRPCServer with an injected Protocol).
 *
 * @details This function facilitates the creation of an RPC server, since it is typically
 * required to create the RPC server by injecting a ProtocolRPCServer, which in its turn requires
 * the injection of a Protocol implementation.
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
 * @brief Factory function that creates a client stack consisting of a ProtocolRPCClient with an
 * injected network client (created by the provided function).
 *
 * @details This function facilitates the creation of an RPC client, since it is typically
 * required to create a ProtocolRPCClient from a network client implementation.
 *
 * @param factory_func Factory function for the network client.
 * @param encoding PayloadEncoding to be used for the ProtocolRPCClient.
 *
 * @return A Protocol implementation, which will be a ProtocolRPCClient.
 */
std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func, PayloadEncoding encoding);

/**
 * @brief Factory function that creates a client stack consisting of a ProtocolRPCClient with an
 * injected network client (created by the provided function).
 *
 * @details This function facilitates the creation of an RPC client, since it is typically
 * required to create a ProtocolRPCClient from a network client implementation.
 *
 * @param factory_func Factory function for the network client.
 * @param config Configuration parameters for the ProtocolRPCClient.
 *
 * @return A Protocol implementation, which will be a ProtocolRPCClient.
 */
std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
  ProtocolRPCClientConfig config);

/**
 * @brief Parse the encoding field of a configuration AnyValue.
 *
 * @param config configuration AnyValue.
 *
 * @return Parsed encoding or PayloadEncoding::kBase64 if no encoding field was present.
 * @throws InvalidOperationException when an encoding field was found but could not be parsed
 * because of wrong type or unknown encoding.
 */
PayloadEncoding ParsePayloadEncoding(const sup::dto::AnyValue& config);

/**
 * @brief Validate the presence of the given field with the given type in the provided
 * configuration AnyValue.
 *
 * @param config configuration AnyValue.
 * @param field_name field name to check.
 * @param anytype expected type of the field.
 *
 * @throws InvalidOperationException when the provided field was not present or of the wrong type.
 */
void ValidateConfigurationField(const sup::dto::AnyValue& config, const std::string& field_name,
                                const sup::dto::AnyType& anytype);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_FACTORY_UTILS_H_
