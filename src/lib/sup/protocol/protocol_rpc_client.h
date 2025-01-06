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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_

#include <sup/dto/any_functor.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_rpc_client_config.h>
#include <sup/protocol/protocol_rpc.h>

#include <tuple>

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
  explicit ProtocolRPCClient(sup::dto::AnyFunctor& any_functor,
                             PayloadEncoding encoding = PayloadEncoding::kBase64);
  ProtocolRPCClient(sup::dto::AnyFunctor& any_functor, ProtocolRPCClientConfig config);
  ~ProtocolRPCClient() override;

  // No copy/move ctor/assignment:
  ProtocolRPCClient(const ProtocolRPCClient&) = delete;
  ProtocolRPCClient(ProtocolRPCClient&&) = delete;
  ProtocolRPCClient& operator=(const ProtocolRPCClient&) = delete;
  ProtocolRPCClient& operator=(ProtocolRPCClient&&) = delete;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  ProtocolResult HandleSyncInvoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output);
  ProtocolResult HandleAsyncInvoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output);
  std::tuple<sup::dto::uint64, ProtocolResult, sup::dto::AnyValue> AsyncSendRequest(
    const sup::dto::AnyValue& input);
  std::pair<bool, ProtocolResult> AsyncPoll(sup::dto::uint64 id);
  std::pair<ProtocolResult, sup::dto::AnyValue> AsyncGetReply(sup::dto::uint64 id);
  sup::dto::AnyFunctor& m_any_functor;
  ProtocolRPCClientConfig m_config;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_H_
