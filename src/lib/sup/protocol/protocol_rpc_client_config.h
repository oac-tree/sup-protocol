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

#ifndef SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_CONFIG_H_
#define SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_CONFIG_H_

#include <sup/protocol/protocol_rpc.h>

namespace sup
{
namespace protocol
{
/**
 * @brief ProtocolRPCClientConfig contains the configuration information of the ProtocolRPCClient.
 *
 * @details The configuration includes:
 *   - The optional encoding to be applied to the payload (none or base64);
 *   - Whether or not the underlying RPC communication will be dealt with asynchronously;
 *   - In case of asynchrounous communication: the total timeout and polling interval in seconds.
 */
struct ProtocolRPCClientConfig
{
public:
  /**
   * @brief The default configuration uses base64 encoding and no asynchronous communication.
   */
  ProtocolRPCClientConfig();
  /**
   * @brief Construct a client configuration with the given encoding and no asynchronous
   * communication.
   *
   * @param encoding Encoding to use (none or base64).
   */
  explicit ProtocolRPCClientConfig(PayloadEncoding encoding);
  /**
   * @brief Construct a client configuration with the given encoding and asynchronous communication
   * with the provided timing paramaters.
   *
   * @param encoding Encoding to use (none or base64).
   * @param timeout_sec Maximum time to wait for the reply to become ready (in seconds).
   * @param polling_interval_sec Time to wait between polls to check if the reply is ready
   * (in seconds).
   */
  ProtocolRPCClientConfig(PayloadEncoding encoding, double timeout_sec,
                          double polling_interval_sec);
  ~ProtocolRPCClientConfig();

  // Copy/move
  ProtocolRPCClientConfig(const ProtocolRPCClientConfig& other);
  ProtocolRPCClientConfig& operator=(const ProtocolRPCClientConfig& other) &;
  ProtocolRPCClientConfig(ProtocolRPCClientConfig&&) noexcept;
  ProtocolRPCClientConfig& operator=(ProtocolRPCClientConfig&&) & noexcept;

  PayloadEncoding m_encoding;
  bool m_async;
  double m_timeout_sec;
  double m_polling_interval_sec;
};

bool ValidateProtocolRPCClientConfig(const ProtocolRPCClientConfig& cfg);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RPC_CLIENT_CONFIG_H_
