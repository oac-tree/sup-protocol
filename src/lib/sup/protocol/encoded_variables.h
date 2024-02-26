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

#ifndef SUP_PROTOCOL_ENCODED_VARIABLES_H_
#define SUP_PROTOCOL_ENCODED_VARIABLES_H_

#include <sup/protocol/base64_variable_codec.h>
#include <sup/protocol/encoded_variable_t.h>

namespace sup
{
namespace protocol
{
/**
 * @brief Base64ProcessVariable uses the Base64VariableCodec to encode/decode the AnyValue.
 */
using Base64ProcessVariable = EncodedProcessVariable<Base64VariableCodec>;

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ENCODED_VARIABLES_H_
