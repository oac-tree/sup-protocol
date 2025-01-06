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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_SERVER_CONFIG_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_SERVER_CONFIG_H_

namespace sup
{
namespace protocol
{
/**
 * @brief ProtocolRPCServerConfig contains the configuration information of the ProtocolRPCServer.
 *
 * @details The configuration includes:
 *   - The time in seconds for requests to expire (each poll will reset the timer)
 */
struct ProtocolRPCServerConfig
{
public:
  /**
   * @brief The default configuration uses a 30 minutes expiration time.
   */
  ProtocolRPCServerConfig();

  /**
   * @brief Construct a server configuration with the given expiration time.
   *
   * @param expiration_sec Expiration time in seconds.
   */
  explicit ProtocolRPCServerConfig(double expiration_sec);

  ~ProtocolRPCServerConfig();

  // Copy/move
  ProtocolRPCServerConfig(const ProtocolRPCServerConfig& other);
  ProtocolRPCServerConfig& operator=(const ProtocolRPCServerConfig& other) &;
  ProtocolRPCServerConfig(ProtocolRPCServerConfig&&) noexcept;
  ProtocolRPCServerConfig& operator=(ProtocolRPCServerConfig&&) & noexcept;

  double m_expiration_sec;
};

bool ValidateProtocolRPCServerConfig(const ProtocolRPCServerConfig& cfg);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_SERVER_CONFIG_H_
