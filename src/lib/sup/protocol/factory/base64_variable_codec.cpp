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

#include <sup/protocol/base64_variable_codec.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/codec/base64.h>

namespace sup
{
namespace protocol
{
std::pair<bool, sup::dto::AnyValue> Base64VariableCodec::Encode(const sup::dto::AnyValue& val)
{
  sup::dto::AnyValue encoded = {{
    { kEncodingField, { sup::dto::StringType, kBase64Encoding }}
  }};
  auto binary = sup::dto::AnyValueToBinary(val);
  (void)encoded.AddMember(kValueField, sup::codec::Base64Encode(binary));
  return { true, encoded };
}

std::pair<bool, sup::dto::AnyValue> Base64VariableCodec::Decode(const sup::dto::AnyValue& val)
{
  if (!ValidateBase64AnyValue(val))
  {
    return { false, {} };
  }
  auto encoded = val[kValueField].As<std::string>();
  try
  {
    auto binary = sup::codec::Base64Decode(encoded);
    return { true, sup::dto::AnyValueFromBinary(binary) };
  }
  catch(const std::exception&)
  {  // Do not handle exception. Result will indicate failure.
  }
  return { false, {} };
}

bool ValidateBase64AnyValue(const sup::dto::AnyValue& val)
{
  if (!val.HasField(kEncodingField))
  {
    return false;
  }
  auto& encoding = val[kEncodingField];
  if (encoding.GetType() != sup::dto::StringType || encoding.As<std::string>() != kBase64Encoding)
  {
    return false;
  }
  if (!val.HasField(kValueField) || val[kValueField].GetType() != sup::dto::StringType)
  {
    return false;
  }
  return true;
}

}  // namespace protocol

}  // namespace sup
