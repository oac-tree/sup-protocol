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

#ifndef SUP_PROTOCOL_PROTOCOL_ENCODINGS_H_
#define SUP_PROTOCOL_PROTOCOL_ENCODINGS_H_

#include <sup/protocol/protocol_rpc.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{
namespace encoding
{

sup::dto::AnyValue Encode(const sup::dto::AnyValue& val, PayloadEncoding encoding);

sup::dto::AnyValue Decode(const sup::dto::AnyValue& val, PayloadEncoding encoding);

sup::dto::AnyValue Base64Encode(const sup::dto::AnyValue& val);

sup::dto::AnyValue Base64Decode(const sup::dto::AnyValue& val);


}  // namespace encoding

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_ENCODINGS_H_
