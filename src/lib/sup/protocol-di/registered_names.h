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

#ifndef SUP_PROTOCOL_DI_REGISTERED_NAMES_H_
#define SUP_PROTOCOL_DI_REGISTERED_NAMES_H_

#include <string>

namespace sup
{
namespace protocol
{

// ProtocolRPCClient registered names:
const std::string PROTOCOL_RPC_CLIENT = "ProtocolRPCClient";

// ProtocolRPCServer registered names:
const std::string PROTOCOL_RPC_SERVER = "ProtocolRPCServer";

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_DI_REGISTERED_NAMES_H_
