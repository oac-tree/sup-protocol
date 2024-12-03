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

#include "polling_timeout_handler.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <thread>

namespace sup
{
namespace protocol
{
namespace
{
sup::dto::uint64 ToNanoseconds(double seconds);
}  // unnamed namespace

PollingTimeoutHandler::PollingTimeoutHandler(double timeout_sec, double polling_interval_sec)
  : m_start_timestamp{GetCurrentTimestamp()}
  , m_timeout_duration_ns{ToNanoseconds(timeout_sec)}
  , m_polling_interval_ns{ToNanoseconds(polling_interval_sec)}
{}

PollingTimeoutHandler::~PollingTimeoutHandler() = default;

bool PollingTimeoutHandler::Wait()
{
  auto now = GetCurrentTimestamp();
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

sup::dto::uint64 GetCurrentTimestamp()
{
  auto now = std::chrono::system_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  return ns;
}

namespace
{
sup::dto::uint64 ToNanoseconds(double seconds)
{
  return std::lround(seconds * 1e9);
}
}  // unnamed namespace


}  // namespace protocol

}  // namespace sup
