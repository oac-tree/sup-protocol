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

#include <sup/protocol/factory/rpc_server_stack.h>

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

}  // namespace protocol

}  // namespace sup
