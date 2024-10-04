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
 * Copyright (c) : 2010-2024 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_PROTOCOL_RPC_CLIENT_STACK_H_
#define SUP_PROTOCOL_RPC_CLIENT_STACK_H_

#include <sup/protocol/protocol.h>
#include <sup/protocol/protocol_rpc_client.h>

#include <functional>
#include <memory>

namespace sup
{
namespace protocol
{

/**
 * @brief RPCClientStack is a Protocol that encapsulates both the client's network implementation
 * and a ProtocolRPCClient.
 */
class RPCClientStack : public Protocol
{
public:
  RPCClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                 PayloadEncoding encoding);

  ~RPCClientStack() override;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override;

private:
  std::unique_ptr<sup::dto::AnyFunctor> m_rpc_client;
  ProtocolRPCClient m_protocol_client;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_RPC_CLIENT_STACK_H_
