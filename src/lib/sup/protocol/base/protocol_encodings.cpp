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

#include "protocol_encodings.h"

#include <sup/protocol/exceptions.h>

#include <sup/dto/anyvalue_helper.h>
#include <sup/codec/base64.h>

#include <functional>
#include <map>

namespace
{
sup::dto::AnyValue Identity(const sup::dto::AnyValue& val)
{
  return val;
}
}  // unnamed namespace

namespace sup
{
namespace protocol
{
namespace encoding
{

sup::dto::AnyValue Encode(const sup::dto::AnyValue& val, PayloadEncoding encoding)
{
  using EncoderFunction = std::function<sup::dto::AnyValue(const sup::dto::AnyValue&)>;
  static const std::map<PayloadEncoding, EncoderFunction> encoder_map = {
    { PayloadEncoding::kNone, Identity },
    { PayloadEncoding::kBase64, Base64Encode }
  };
  auto it = encoder_map.find(encoding);
  if (it == encoder_map.end())
  {
    std::string error = "encoding::Encode(): unknown encoding type: " +
                        std::to_string(static_cast<int>(encoding));
    throw InvalidOperationException(error);
  }
  return it->second(val);
}

sup::dto::AnyValue Decode(const sup::dto::AnyValue& val, PayloadEncoding encoding)
{
  using DecoderFunction = std::function<sup::dto::AnyValue(const sup::dto::AnyValue&)>;
  static const std::map<PayloadEncoding, DecoderFunction> decoder_map = {
    { PayloadEncoding::kNone, Identity },
    { PayloadEncoding::kBase64, Base64Decode }
  };
  auto it = decoder_map.find(encoding);
  if (it == decoder_map.end())
  {
    std::string error = "encoding::Decode(): unknown encoding type: " +
                        std::to_string(static_cast<int>(encoding));
    throw InvalidOperationException(error);
  }
  return it->second(val);
}

sup::dto::AnyValue Base64Encode(const sup::dto::AnyValue& val)
{
  auto binary = sup::dto::AnyValueToBinary(val);
  return sup::codec::Base64Encode(binary);
}

sup::dto::AnyValue Base64Decode(const sup::dto::AnyValue& val)
{
  if (val.GetType() != sup::dto::StringType)
  {
    throw InvalidOperationException("Base64Decode(): passed value was not of string type");
  }
  auto binary = sup::codec::Base64Decode(val.As<std::string>());
  return sup::dto::AnyValueFromBinary(binary);
}

}  // namespace encoding

}  // namespace protocol

}  // namespace sup

