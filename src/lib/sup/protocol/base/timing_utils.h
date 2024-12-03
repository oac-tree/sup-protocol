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

#ifndef SUP_PROTOCOL_TIMING_UTILS_H_
#define SUP_PROTOCOL_TIMING_UTILS_H_

#include <sup/dto/basic_scalar_types.h>

namespace sup
{
namespace protocol
{
namespace utils
{
sup::dto::uint64 GetCurrentTimestamp();

sup::dto::uint64 ToNanoseconds(double seconds);

}  // namespace utils

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_TIMING_UTILS_H_
