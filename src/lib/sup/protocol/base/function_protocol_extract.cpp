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

#include <sup/protocol/function_protocol_extract.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/dto/json_type_parser.h>

namespace
{
const sup::dto::AnyType STRING_ARRAY_TYPE{0, sup::dto::StringType};
}  // unnamed namespace

namespace sup
{
namespace protocol
{

template <typename T>
sup::dto::AnyType FunctionProtocolExpectedType()
{
  return sup::dto::EmptyType;
}

template <>
sup::dto::AnyType FunctionProtocolExpectedType<bool>()
{
  return sup::dto::BooleanType;
}

template <>
sup::dto::AnyType FunctionProtocolExpectedType<std::string>()
{
  return sup::dto::StringType;
}

template <>
sup::dto::AnyType FunctionProtocolExpectedType<std::vector<std::string>>()
{
  return STRING_ARRAY_TYPE;
}

template <>
sup::dto::AnyType FunctionProtocolExpectedType<sup::dto::AnyType>()
{
  return sup::dto::StringType;
}

template <typename T>
bool ValidateFunctionProtocolInput(const sup::dto::AnyValue& input, const std::string& field_name)
{
  if (!input.HasField(field_name))
  {
    return false;
  }
  auto expected_type = FunctionProtocolExpectedType<T>();
  // For array types, we only check the element type, not the number of elements
  if (sup::dto::IsArrayType(expected_type))
  {
    const auto& field = input[field_name];
    if (!sup::dto::IsArrayValue(field) ||
         field.GetType().ElementType() != expected_type.ElementType())
    {
      return false;
    }
  }
  else
  {
    if (input[field_name].GetType() != expected_type)
    {
      return false;
    }
  }
  return true;
}

template <typename T>
bool FunctionProtocolExtractT(T& val, const sup::dto::AnyValue& input,
                              const std::string& field_name)
{
  if (!ValidateFunctionProtocolInput<T>(input, field_name))
  {
    return false;
  }
  val = input[field_name].As<T>();
  return true;
}

bool FunctionProtocolExtract(bool& b, const sup::dto::AnyValue& input,
                             const std::string& field_name)
{
  return FunctionProtocolExtractT(b, input, field_name);
}

bool FunctionProtocolExtract(std::string& str, const sup::dto::AnyValue& input,
                             const std::string& field_name)
{
  return FunctionProtocolExtractT(str, input, field_name);
}

bool FunctionProtocolExtract(std::vector<std::string>& string_list,
                             const sup::dto::AnyValue& input,
                             const std::string& field_name)
{
  if (!ValidateFunctionProtocolInput<std::vector<std::string>>(input, field_name))
  {
    return false;
  }
  const auto& field = input[field_name];
  string_list.clear();
  for (size_t i = 0; i < field.NumberOfElements(); ++i)
  {
    string_list.push_back(field[i].As<std::string>());
  }
  return true;
}

bool FunctionProtocolExtract(sup::dto::AnyValue& anyvalue, const sup::dto::AnyValue& input,
                             const std::string& field_name)
{
  if (!input.HasField(field_name))
  {
    return false;
  }
  if (!sup::dto::TryAssignIfEmptyOrConvert(anyvalue, input[field_name]))
  {
    return false;
  }
  return true;
}

bool FunctionProtocolExtract(sup::dto::AnyType& anytype, const sup::dto::AnyValue& input,
                             const std::string& field_name)
{
  if (!ValidateFunctionProtocolInput<sup::dto::AnyType>(input, field_name))
  {
    return false;
  }
  auto json_type = input[field_name].As<std::string>();
  sup::dto::JSONAnyTypeParser parser{};
  if (!parser.ParseString(json_type))
  {
    return false;
  }
  anytype = parser.MoveAnyType();
  return true;
}

}  // namespace protocol

}  // namespace sup
