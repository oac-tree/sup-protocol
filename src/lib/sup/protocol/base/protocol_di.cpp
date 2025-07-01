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

#include <sup/protocol/registered_names.h>

#include <sup/protocol/protocol_rpc_client.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <sup/di/object_manager.h>

using namespace sup::protocol;

// Register ProtocolRPCClient

const bool ProtocolRPCClient_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PROTOCOL_RPC_CLIENT,
    sup::di::ForwardingInstanceFactoryFunction<Protocol, ProtocolRPCClient,
                                               sup::dto::AnyFunctor&>);

// Register ProtocolRPCServer

const bool ProtocolRPCServer_Registered =
  sup::di::GlobalObjectManager().RegisterFactoryFunction(PROTOCOL_RPC_SERVER,
    sup::di::ForwardingInstanceFactoryFunction<sup::dto::AnyFunctor, ProtocolRPCServer,
                                               Protocol&>);
