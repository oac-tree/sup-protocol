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

#include <sup/protocol/factory/rpc_client_stack.h>

namespace sup
{
namespace protocol
{

RPCClientStack::RPCClientStack(std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func,
                               PayloadEncoding encoding)
  : m_rpc_client{factory_func()}
  , m_protocol_client{*m_rpc_client, encoding}
{}

RPCClientStack::~RPCClientStack() = default;

ProtocolResult RPCClientStack::Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  return m_protocol_client.Invoke(input, output);
}

ProtocolResult RPCClientStack::Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output)
{
  return m_protocol_client.Service(input, output);
}

}  // namespace protocol

}  // namespace sup
