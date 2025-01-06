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

#include <sup/protocol/variable_callback_guard.h>

namespace sup
{
namespace protocol
{
VariableCallbackGuard::VariableCallbackGuard(ProcessVariable& var, ProcessVariable::Callback cb)
  : m_var{var}, m_cb_supported{}
{
  m_cb_supported = m_var.SetMonitorCallback(cb);
}

VariableCallbackGuard::~VariableCallbackGuard()
{
  if (m_cb_supported)
  {
    (void)m_var.SetMonitorCallback({});
  }
}

bool VariableCallbackGuard::CallbackSupported() const
{
  return m_cb_supported;
}

}  // namespace protocol

}  // namespace sup
