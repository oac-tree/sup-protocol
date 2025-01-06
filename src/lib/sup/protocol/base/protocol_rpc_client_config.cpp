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

#include <sup/protocol/protocol_rpc_client_config.h>

namespace sup
{
namespace protocol
{

ProtocolRPCClientConfig::ProtocolRPCClientConfig()
  : m_encoding{PayloadEncoding::kBase64}
  , m_async{false}
  , m_timeout_sec{}
  , m_polling_interval_sec{}
{}

ProtocolRPCClientConfig::ProtocolRPCClientConfig(PayloadEncoding encoding)
  : m_encoding{encoding}
  , m_async{false}
  , m_timeout_sec{}
  , m_polling_interval_sec{}
{}

ProtocolRPCClientConfig::ProtocolRPCClientConfig(PayloadEncoding encoding, double timeout_sec,
                                                 double polling_interval_sec)
  : m_encoding{encoding}
  , m_async{true}
  , m_timeout_sec{timeout_sec}
  , m_polling_interval_sec{polling_interval_sec}
{}

ProtocolRPCClientConfig::~ProtocolRPCClientConfig() = default;

ProtocolRPCClientConfig::ProtocolRPCClientConfig(const ProtocolRPCClientConfig&) = default;

ProtocolRPCClientConfig& ProtocolRPCClientConfig::operator=(
  const ProtocolRPCClientConfig&) & = default;

ProtocolRPCClientConfig::ProtocolRPCClientConfig(ProtocolRPCClientConfig&&) noexcept = default;

ProtocolRPCClientConfig& ProtocolRPCClientConfig::operator=(
  ProtocolRPCClientConfig&&) & noexcept = default;

bool ValidateProtocolRPCClientConfig(const ProtocolRPCClientConfig& cfg)
{
  if (!utils::IsSupportedPayloadEncoding(cfg.m_encoding))
  {
    return false;
  }
  if (cfg.m_async)
  {
    auto positive_timeout = (cfg.m_timeout_sec > 0.0);
    auto positive_polling_interval = (cfg.m_polling_interval_sec > 0.0);
    return positive_timeout && positive_polling_interval;
  }
  return true;
}

}  // namespace protocol

}  // namespace sup
