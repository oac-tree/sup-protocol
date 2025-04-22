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

#include "test_process_variable.h"

#include <sup/dto/anyvalue_helper.h>

namespace sup
{
namespace protocol
{
namespace test
{

TestProcessVariable::TestProcessVariable(const sup::dto::AnyValue& val, bool available)
  : m_value{val}
  , m_available{available}
  , m_received_timeout{}
  , m_cb{}
  , m_mtx{}
{}

TestProcessVariable::~TestProcessVariable() = default;

bool TestProcessVariable::IsAvailable() const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_available;
}

std::pair<bool, sup::dto::AnyValue> TestProcessVariable::GetValue(double timeout_sec) const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_received_timeout = timeout_sec;
  return { m_available, m_value };
}

bool TestProcessVariable::SetValue(const sup::dto::AnyValue& value, double timeout_sec)
{
  bool result{};
  {
    std::lock_guard<std::mutex> lk{m_mtx};
    m_received_timeout = timeout_sec;
    if (!m_available)
    {
      return false;
    }
    result = sup::dto::TryAssignIfEmptyOrConvert(m_value, value);
  }
  if (result && m_cb)
  {
    m_cb(value, true);
  }
  return result;
}

bool TestProcessVariable::WaitForAvailable(double timeout_sec) const
{
  std::lock_guard<std::mutex> lk{m_mtx};
  return m_available;
}

bool TestProcessVariable::SetMonitorCallback(Callback func)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  m_cb = func;
  return true;
}

}  // namespace test

}  // namespace protocol

}  // namespace sup
