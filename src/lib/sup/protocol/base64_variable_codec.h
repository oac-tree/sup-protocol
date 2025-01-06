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

#ifndef SUP_PROTOCOL_BASE64_VARIABLE_CODEC_H_
#define SUP_PROTOCOL_BASE64_VARIABLE_CODEC_H_

#include <sup/dto/anyvalue.h>

#include <utility>

namespace sup
{
namespace protocol
{
const std::string kEncodingField = "encoding";
const std::string kBase64Encoding = "base64";
const std::string kValueField = "value";

/**
 * @brief Base64 codec for EncodedProcessVariable.
 *
 * @details It encodes an AnyValue into a base64 string of its binary encoding and puts this into
 * an AnyValue structure with the following fields:
 *   - "encoding": "base64",
 *   - "value": base64 string representation.
 * Decoding is the inverse operation.
 */
class Base64VariableCodec
{
public:
  static std::pair<bool, sup::dto::AnyValue> Encode(const sup::dto::AnyValue& val);
  static std::pair<bool, sup::dto::AnyValue> Decode(const sup::dto::AnyValue& val);
};

/**
 * @brief Check if the provided AnyValue has the correct format for a base64 encoded AnyValue.
 *
 * @param val AnyValue to check.
 * @return true when format is correct, false otherwise.
 */
bool ValidateBase64AnyValue(const sup::dto::AnyValue& val);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_BASE64_VARIABLE_CODEC_H_
