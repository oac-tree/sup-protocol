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

#include "protocol_factory_utils.h"

namespace sup
{
namespace protocol
{

RPCServerStack::RPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol)
  : m_protocol_server{protocol}
  , m_rpc_server{factory_func(m_protocol_server)}
{}

RPCServerStack::~RPCServerStack() = default;

std::unique_ptr<RPCServerInterface> CreateRPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol)
{
  return std::unique_ptr<RPCServerInterface>(new RPCServerStack(factory_func, protocol));
}

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

std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func, PayloadEncoding encoding)
{
  return std::unique_ptr<Protocol>(new RPCClientStack(factory_func, encoding));
}

}  // namespace protocol

}  // namespace sup
