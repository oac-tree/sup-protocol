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

#ifndef SUP_PROTOCOL_PROCESS_VARIABLE_H_
#define SUP_PROTOCOL_PROCESS_VARIABLE_H_

#include <sup/protocol/protocol.h>

#include <memory>

namespace sup
{
namespace protocol
{
/**
 * @brief ProcessVariable is an interface for process variables that are typically accessible over
 * the network (although implementations may decide to implement them locally).
 */
class ProcessVariable
{
public:
  virtual ~ProcessVariable();
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROCESS_VARIABLE_H_
