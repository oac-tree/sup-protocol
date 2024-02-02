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

#ifndef SUP_PROTOCOL_EXCEPTIONS_H_
#define SUP_PROTOCOL_EXCEPTIONS_H_

#include <exception>
#include <string>

namespace sup
{
namespace protocol
{

  /**
 * @brief Base Exception class with message.
 */
class MessageException : public std::exception
{
public:
  explicit MessageException(std::string message);
  const char* what() const noexcept override;
private:
  std::string m_message;
};

/**
 * @brief Exception thrown when performing an operation that is not allowed.
 */
class InvalidOperationException : public MessageException
{
public:
  explicit InvalidOperationException(const std::string& message);
};

/**
 * @brief Exception thrown when trying to access a ProcessVariable's value when it's not available.
 */
class VariableUnavailableException : public MessageException
{
public:
  explicit VariableUnavailableException(const std::string& message);
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_EXCEPTIONS_H_
