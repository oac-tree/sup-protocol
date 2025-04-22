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

#ifndef SUP_PROTOCOL_ANYVALUE_UTILS_H_
#define SUP_PROTOCOL_ANYVALUE_UTILS_H_

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{
bool ValidateMemberType(const sup::dto::AnyValue& value, const std::string& member_name,
                        const sup::dto::AnyType& member_type);

bool ValidateMemberTypeIfPresent(const sup::dto::AnyValue& value, const std::string& member_name,
                                 const sup::dto::AnyType& member_type);
}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ANYVALUE_UTILS_H_
