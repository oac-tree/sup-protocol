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

#include "timing_utils.h"

#include <chrono>
#include <cmath>

namespace sup
{
namespace protocol
{
namespace utils
{

sup::dto::uint64 GetCurrentTimestamp()
{
  auto now = std::chrono::system_clock::now();
  auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  return static_cast<sup::dto::uint64>(ns);
}

sup::dto::uint64 ToNanoseconds(double seconds)
{
  return static_cast<sup::dto::uint64>(std::lround(seconds * 1e9));
}

}  // namespace utils

}  // namespace protocol

}  // namespace sup
