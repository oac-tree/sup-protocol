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

#include "process_variable_utils.h"

#include <chrono>
#include <cmath>
#include <thread>

namespace sup
{
namespace protocol
{
namespace utils
{
void TryFetchVariable(const ProcessVariable& var, sup::dto::AnyValue& output)
{
  auto info = var.GetValue(0.0);
  if (info.first)
  {
    output = info.second;
  }
}

bool BusyWaitForValue(const ProcessVariable& var, const sup::dto::AnyValue& expected_value,
                      double timeout_sec)
{
  auto end_time = std::chrono::system_clock::now() + std::chrono::duration<double>(timeout_sec);
  sup::dto::AnyValue current;
  TryFetchVariable(var, current);
  while (current != expected_value)
  {
    if (std::chrono::system_clock::now() > end_time)
    {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    TryFetchVariable(var, current);
  }
  return true;
}

}  // namespace utils

}  // namespace protocol

}  // namespace sup
