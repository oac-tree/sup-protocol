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

#include <sup/protocol/process_variable.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/variable_callback_guard.h>

#include <sup/dto/anyvalue.h>

#include <chrono>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <thread>

namespace
{
using namespace sup::protocol;
void TryFetchVariable(const ProcessVariable& var, sup::dto::AnyValue& output);
bool BusyWaitForValue(const ProcessVariable& var, const sup::dto::AnyValue& expected_value,
                      double timeout_sec);
}  // unnamed namespace

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
  sup::dto::AnyValue current;
  auto callback = [&current, &mtx, &cv](const sup::dto::AnyValue& updated, bool connected){
    if (connected)
    {
      std::lock_guard<std::mutex> lk(mtx);
      current = updated;
    }
    cv.notify_one();
  };
  VariableCallbackGuard cb_guard(var, callback);
  if (!cb_guard.CallbackSupported())
  {
    return BusyWaitForValue(var, value, timeout_sec);
  }
  std::unique_lock<std::mutex> lk(mtx);
  TryFetchVariable(var, current);
  return cv.wait_for(lk, std::chrono::nanoseconds(std::lround(timeout_sec * 1e9)), [&](){
    return current == value;
  });
}

}  // namespace protocol

}  // namespace sup

namespace
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
  auto end_time =
      std::chrono::system_clock::now() + std::chrono::nanoseconds(std::lround(timeout_sec * 1e9));
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
}  // unnamed namespace

