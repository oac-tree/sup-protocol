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

#include <sup/protocol/process_variable.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/variable_callback_guard.h>
#include <sup/protocol/factory/process_variable_utils.h>

#include <sup/dto/anyvalue.h>

#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <mutex>

namespace sup
{
namespace protocol
{

ProcessVariable::~ProcessVariable() = default;

sup::dto::AnyValue GetVariableValue(const ProcessVariable& var)
{
  auto info = var.GetValue(0.0);
  if (!info.first)
  {
    std::string error = "Could not get ProcessVariable's value";
    throw VariableUnavailableException(error);
  }
  return info.second;
}

bool SetVariableValue(ProcessVariable& var, const sup::dto::AnyValue& value)
{
  return var.SetValue(value, 0.0);
}

bool WaitForVariableValue(ProcessVariable& var, const sup::dto::AnyValue& value,
                          double timeout_sec)
{
  std::mutex mtx;
  std::condition_variable cv;
  bool result{true};
  auto callback = [&cv, &result, &mtx](const sup::dto::AnyValue&, bool connected) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      result = connected;
    }
    cv.notify_one();
  };
  VariableCallbackGuard cb_guard(var, callback);
  if (!cb_guard.CallbackSupported())
  {
    return utils::BusyWaitForValue(var, value, timeout_sec);
  }
  auto pred = [&result]() {
    return result;
  };
  auto finish = std::chrono::system_clock::now() + std::chrono::duration<double>(timeout_sec);
  while (true)
  {
    std::unique_lock<std::mutex> lk(mtx);
    auto success = cv.wait_until(lk, finish, pred);
    // Needed to ensure the next waits will not return true immediately and cause an infinite loop:
    result = false;
    lk.unlock();
    if (!success)
    {
      break;
    }
    auto read_back = var.GetValue(0.0);
    if (read_back.first && read_back.second == value)
    {
      return true;
    }
  }
  return false;
}

}  // namespace protocol

}  // namespace sup
