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

#include <sup/protocol/exceptions.h>

namespace sup
{
namespace protocol
{

MessageException::MessageException(std::string message)
  : std::exception(), m_message{std::move(message)}
{}

const char* MessageException::what() const noexcept
{
  return m_message.c_str();
}

InvalidOperationException::InvalidOperationException(const std::string& message)
  : MessageException{message}
{}

VariableUnavailableException::VariableUnavailableException(const std::string& message)
  : MessageException{message}
{}

}  // namespace protocol

}  // namespace sup
