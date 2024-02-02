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

#include <sup/protocol/protocol_factory_utils.h>

#include <sup/protocol/factory/rpc_client_stack.h>
#include <sup/protocol/factory/rpc_server_stack.h>

namespace sup
{
namespace protocol
{

std::unique_ptr<RPCServerInterface> CreateRPCServerStack(
  std::function<std::unique_ptr<RPCServerInterface>(sup::dto::AnyFunctor&)> factory_func,
  Protocol& protocol)
{
  return std::unique_ptr<RPCServerInterface>(new RPCServerStack(factory_func, protocol));
}

std::unique_ptr<Protocol> CreateRPCClientStack(
  std::function<std::unique_ptr<sup::dto::AnyFunctor>()> factory_func, PayloadEncoding encoding)
{
  return std::unique_ptr<Protocol>(new RPCClientStack(factory_func, encoding));
}

}  // namespace protocol

}  // namespace sup
