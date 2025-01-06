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

#include "polling_timeout_handler.h"
#include "timing_utils.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace sup
{
namespace protocol
{
PollingTimeoutHandler::PollingTimeoutHandler(double timeout_sec, double polling_interval_sec)
  : m_start_timestamp{utils::GetCurrentTimestamp()}
  , m_timeout_duration_ns{utils::ToNanoseconds(timeout_sec)}
  , m_polling_interval_ns{utils::ToNanoseconds(polling_interval_sec)}
{}

PollingTimeoutHandler::~PollingTimeoutHandler() = default;

PollingTimeoutHandler::PollingTimeoutHandler(const PollingTimeoutHandler&) = default;

PollingTimeoutHandler& PollingTimeoutHandler::operator=(
  const PollingTimeoutHandler&) & = default;

PollingTimeoutHandler::PollingTimeoutHandler(PollingTimeoutHandler&&) noexcept = default;

PollingTimeoutHandler& PollingTimeoutHandler::operator=(
  PollingTimeoutHandler&&) & noexcept = default;

bool PollingTimeoutHandler::Wait()
{
  auto now = utils::GetCurrentTimestamp();
  auto passed_ns = now - m_start_timestamp;
  if (passed_ns > m_timeout_duration_ns)
  {
    return false;
  }
  // Do not sleep so long as to pass the timeout threshold:
  auto sleep_time = std::min(m_polling_interval_ns, m_timeout_duration_ns - passed_ns);
  std::this_thread::sleep_for(std::chrono::nanoseconds(sleep_time));
  return true;
}

}  // namespace protocol

}  // namespace sup
