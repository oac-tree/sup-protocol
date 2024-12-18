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

#include <sup/protocol/protocol_rpc_server_config.h>

namespace sup
{
namespace protocol
{

ProtocolRPCServerConfig::ProtocolRPCServerConfig()
  : m_expiration_sec{1800.0}
{}

ProtocolRPCServerConfig::ProtocolRPCServerConfig(double expiration_sec)
  : m_expiration_sec{expiration_sec}
{}

ProtocolRPCServerConfig::~ProtocolRPCServerConfig() = default;

ProtocolRPCServerConfig::ProtocolRPCServerConfig(const ProtocolRPCServerConfig&) = default;

ProtocolRPCServerConfig& ProtocolRPCServerConfig::operator=(
  const ProtocolRPCServerConfig&) & = default;

ProtocolRPCServerConfig::ProtocolRPCServerConfig(ProtocolRPCServerConfig&&) noexcept = default;

ProtocolRPCServerConfig& ProtocolRPCServerConfig::operator=(
  ProtocolRPCServerConfig&&) & noexcept = default;

bool ValidateProtocolRPCServerConfig(const ProtocolRPCServerConfig& cfg)
{
  return cfg.m_expiration_sec > 0.0;
}

}  // namespace protocol

}  // namespace sup
