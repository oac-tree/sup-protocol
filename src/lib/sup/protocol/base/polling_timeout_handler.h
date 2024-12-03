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

#ifndef SUP_PROTOCOL_POLLING_TIMEOUT_HANDLER_H_
#define SUP_PROTOCOL_POLLING_TIMEOUT_HANDLER_H_

#include <sup/dto/basic_scalar_types.h>

namespace sup
{
namespace protocol
{
/**
 * @brief This class the timeout and polling intervals for asynchronous polling between a client
 * and server.
 */
class PollingTimeoutHandler
{
public:

  /**
   * @brief Constructor. During construction, the current time will be registered to allow the
   * object to know when the timeout was exceeded.
   *
   * @param timeout_sec Maximum time to wait for the reply to become ready (in seconds).
   * @param polling_interval_sec Time to wait between polls to check if the reply is ready
   */
  PollingTimeoutHandler(double timeout_sec, double polling_interval_sec);
  ~PollingTimeoutHandler();

  /**
   * @brief Wait for the next moment to poll.
   *
   * @return true if the timeout was not exceeded yet.
   */
  bool Wait();

private:
  sup::dto::uint64 m_start_timestamp;
  sup::dto::uint64 m_timeout_duration_ns;
  sup::dto::uint64 m_polling_interval_ns;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_POLLING_TIMEOUT_HANDLER_H_
