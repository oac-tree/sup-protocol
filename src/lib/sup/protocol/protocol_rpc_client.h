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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_

#include <sup/dto/any_functor.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace protocol
{
/**
 * @brief The ProtocolRPCClient is a Protocol implementation that forwards to an AnyFunctor.
 *
 * @details The ProtocolRPCClient is intended to be used as a mediator between the Protocol
 * and a concrete RPC client class that derives from AnyFunctor.
 *
 * It forwards the input as a payload of an RPC request into the AnyFunctor and extracts the
 * payload from the RPC reply (see protocol_rpc.h).
 */
class ProtocolRPCClient : public Protocol
{
public:
  ProtocolRPCClient(dto::AnyFunctor& any_functor);
  ~ProtocolRPCClient();

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  dto::AnyFunctor& m_any_functor;
  PayloadEncoding m_encoding;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_
