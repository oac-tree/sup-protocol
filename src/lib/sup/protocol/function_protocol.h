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

#ifndef SUP_PROTOCOL_FUNCTION_PROTOCOL_H_
#define SUP_PROTOCOL_FUNCTION_PROTOCOL_H_

#include <sup/protocol/protocol_result.h>

#include <sup/dto/anyvalue.h>

#include <map>
#include <string>

namespace sup
{
namespace protocol
{

const std::string FUNCTION_FIELD_NAME = "function";

sup::dto::AnyValue FunctionProtocolInput(const std::string& func_name);

template <typename T>
using ProtocolMemberFunction = ProtocolResult(T::*)(const sup::dto::AnyValue&, sup::dto::AnyValue&);

template <typename T>
using ProtocolMemberFunctionMap = std::map<std::string, ProtocolMemberFunction<T>>;

template <typename T>
ProtocolResult CallFunctionProtocol(T& obj, const ProtocolMemberFunctionMap<T>& func_map,
                                    const sup::dto::AnyValue& input, sup::dto::AnyValue& output,
                                    const ProtocolResult& not_found_result)
{
  if (!input.HasField(FUNCTION_FIELD_NAME) ||
      input[FUNCTION_FIELD_NAME].GetType() != sup::dto::StringType)
  {
    return not_found_result;
  }
  auto func_name = input[FUNCTION_FIELD_NAME].As<std::string>();
  auto it = func_map.find(func_name);
  if (it == func_map.end())
  {
    return not_found_result;
  }
  auto mem_fun = it->second;
  return (obj.*mem_fun)(input, output);
}

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_FUNCTION_PROTOCOL_H_
