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

#ifndef SUP_PROTOCOL_FUNCTION_PROTOCOL_PACK_H_
#define SUP_PROTOCOL_FUNCTION_PROTOCOL_PACK_H_

#include <sup/dto/anytype.h>
#include <sup/dto/anyvalue.h>

#include <string>
#include <vector>

namespace sup
{
namespace protocol
{

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name, bool b);

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name,
                          const std::string& str);

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name,
                          const char* str);

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name,
                          const std::vector<std::string>& string_list);

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name,
                          const sup::dto::AnyValue& anyvalue);

void FunctionProtocolPack(sup::dto::AnyValue& output, const std::string& field_name,
                          const sup::dto::AnyType& anytype);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_FUNCTION_PROTOCOL_PACK_H_
