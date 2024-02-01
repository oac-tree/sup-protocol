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

#include <sup/protocol/function_protocol.h>

namespace sup
{
namespace protocol
{

sup::dto::AnyValue FunctionProtocolInput(const std::string& func_name)
{
  sup::dto::AnyValue result = sup::dto::EmptyStruct();
  result.AddMember(FUNCTION_FIELD_NAME, func_name);
  return result;
}

}  // namespace protocol

}  // namespace sup
