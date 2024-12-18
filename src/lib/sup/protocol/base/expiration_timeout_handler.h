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

#ifndef SUP_PROTOCOL_EXPIRATION_TIMEOUT_HANDLER_H_
#define SUP_PROTOCOL_EXPIRATION_TIMEOUT_HANDLER_H_

#include <sup/dto/basic_scalar_types.h>

namespace sup
{
namespace protocol
{
/**
 * @brief This class manages the expiration timeouts for a server. It can be used to not try to
 * cleanup resources on each and every request, but only when a certain time has expired.
 */
class ExpirationTimeoutHandler
{
public:

  /**
   * @brief Constructor.
   *
   * @param cleanup_sec Minimum time to wait between consecutive cleanup operations in the server.
   */
  explicit ExpirationTimeoutHandler(double cleanup_sec);
  ~ExpirationTimeoutHandler();

  ExpirationTimeoutHandler(const ExpirationTimeoutHandler& other);
  ExpirationTimeoutHandler& operator=(const ExpirationTimeoutHandler& other) &;
  ExpirationTimeoutHandler(ExpirationTimeoutHandler&&) noexcept;
  ExpirationTimeoutHandler& operator=(ExpirationTimeoutHandler&&) & noexcept;

  /**
   * @brief Check if a new clean up is needed, based on the time expired from the last clean up.
   *
   * @note When a new clean up is needed, this object will also update the last clean up time.
   *
   * @return true if a new clean up is needed.
   */
  bool IsCleanUpNeeded();

private:
  sup::dto::uint64 m_last_timestamp;
  sup::dto::uint64 m_cleanup_ns;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_EXPIRATION_TIMEOUT_HANDLER_H_
