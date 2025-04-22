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

#ifndef SUP_PROTOCOL_VARIABLE_CALLBACK_GUARD_H_
#define SUP_PROTOCOL_VARIABLE_CALLBACK_GUARD_H_

#include <sup/protocol/process_variable.h>

namespace sup
{
namespace protocol
{

/**
 * @brief This class implements RAII for callbacks on a ProcessVariable. It tries to set a callback
 * on a ProcessVariable and will remove it upon destruction.
 *
 * @details Users can query if callbacks were supported by the provided ProcessVariable by calling
 * the `CallbackSupported` member function.
 *
 */
class VariableCallbackGuard
{
public:
  VariableCallbackGuard(ProcessVariable& var, ProcessVariable::Callback cb);
  ~VariableCallbackGuard();

  bool CallbackSupported() const;

  VariableCallbackGuard(const VariableCallbackGuard&) = delete;
  VariableCallbackGuard(VariableCallbackGuard&&) = delete;
  VariableCallbackGuard& operator=(const VariableCallbackGuard&) = delete;
  VariableCallbackGuard& operator=(VariableCallbackGuard&&) = delete;

private:
  ProcessVariable& m_var;
  bool m_cb_supported;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_VARIABLE_CALLBACK_GUARD_H_
