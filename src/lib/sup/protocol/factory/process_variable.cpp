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
 * Copyright (c) : 2010-2023 ITER Organization,
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

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{

ProcessVariable::~ProcessVariable() = default;

sup::dto::AnyValue GetVariableValue(const ProcessVariable& var)
{
  return var.GetValue(0.0);
}

bool SetVariableValue(ProcessVariable& var, const sup::dto::AnyValue& value)
{
  return var.SetValue(value, 0.0);
}

}  // namespace protocol

}  // namespace sup
