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

#include "expiration_timeout_handler.h"
#include "timing_utils.h"

namespace sup
{
namespace protocol
{
ExpirationTimeoutHandler::ExpirationTimeoutHandler(double cleanup_sec)
  : m_last_timestamp{0}
  , m_cleanup_ns{utils::ToNanoseconds(cleanup_sec)}
{}

ExpirationTimeoutHandler::~ExpirationTimeoutHandler() = default;

ExpirationTimeoutHandler::ExpirationTimeoutHandler(const ExpirationTimeoutHandler&) = default;

ExpirationTimeoutHandler& ExpirationTimeoutHandler::operator=(
  const ExpirationTimeoutHandler&) & = default;

ExpirationTimeoutHandler::ExpirationTimeoutHandler(ExpirationTimeoutHandler&&) noexcept = default;

ExpirationTimeoutHandler& ExpirationTimeoutHandler::operator=(
  ExpirationTimeoutHandler&&) & noexcept = default;

bool ExpirationTimeoutHandler::IsCleanUpNeeded()
{
  auto now = utils::GetCurrentTimestamp();
  auto passed_ns = now - m_last_timestamp;
  if (passed_ns > m_cleanup_ns)
  {
    m_last_timestamp = now;
    return true;
  }
  return false;
}

}  // namespace protocol

}  // namespace sup
