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

#ifndef SUP_PROTOCOL_ENCODED_VARIABLE_T_H_
#define SUP_PROTOCOL_ENCODED_VARIABLE_T_H_

#include <sup/protocol/exceptions.h>
#include <sup/protocol/process_variable.h>

namespace sup
{
namespace protocol
{
/**
 * @brief ProcessVariable decorator that encodes/decodes the AnyValue according to the template
 * parameter Codec:
 *
 * @code {.cpp}
 * class MyCodec
 * {
 * public:
 *   static std::pair<bool, AnyValue> Encode(const AnyValue& val);
 *   static std::pair<bool, AnyValue> Decode(const AnyValue& val);
 * };
 * @endcode
 *
 */
template <typename Codec>
class EncodedProcessVariable : public ProcessVariable
{
public:
  explicit EncodedProcessVariable(std::unique_ptr<ProcessVariable> variable);

  virtual ~EncodedProcessVariable() = default;

  /**
   * @brief Forward to underlying variable.
   */
  bool IsAvailable() const override;

  /**
   * @brief Retrieve and decode the variable's value.
   *
   * @note Failure to decode will set the boolean to false.
   */
  std::pair<bool, sup::dto::AnyValue> GetValue(double timeout_sec) const override;

  /**
   * @brief Encode and write a value to the variable.
   */
  bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) override;

  /**
   * @brief Forward to underlying variable.
   */
  bool WaitForAvailable(double timeout_sec) const override;

  /**
   * @brief Set a callback for value/availability updates that includes decoding.
   */
  bool SetMonitorCallback(Callback func) override;

private:
  std::unique_ptr<ProcessVariable> m_variable;
};

template <typename Codec>
EncodedProcessVariable<Codec>::EncodedProcessVariable(std::unique_ptr<ProcessVariable> variable)
  : m_variable{std::move(variable)}
{
  if (!m_variable)
  {
    const std::string error = "EncodedProcessVariable constructor called with nullptr variable";
    throw InvalidOperationException(error);
  }
}

template <typename Codec>
bool EncodedProcessVariable<Codec>::IsAvailable() const
{
  return WaitForAvailable(0.0);
}

template <typename Codec>
std::pair<bool, sup::dto::AnyValue> EncodedProcessVariable<Codec>::GetValue(double timeout_sec) const
{
  auto encoded = m_variable->GetValue(timeout_sec);
  if (!encoded.first)
  {
    return { false, {} };
  }
  return Codec::Decode(encoded.second);
}

template <typename Codec>
bool EncodedProcessVariable<Codec>::SetValue(const sup::dto::AnyValue& value, double timeout_sec)
{
  auto encoded = Codec::Encode(value);
  if (!encoded.first)
  {
    return false;
  }
  return m_variable->SetValue(encoded.second, timeout_sec);
}

template <typename Codec>
bool EncodedProcessVariable<Codec>::WaitForAvailable(double timeout_sec) const
{
  auto encoded = m_variable->GetValue(timeout_sec);
  if (!encoded.first)
  {
    return false;
  }
  auto decoded = Codec::Decode(encoded.second);
  return decoded.first;
}

template <typename Codec>
bool EncodedProcessVariable<Codec>::SetMonitorCallback(Callback func)
{
  auto wrapped_cb = [func](const sup::dto::AnyValue& value, bool available)
  {
    auto decoded = Codec::Decode(value);
    func(decoded.second, available && decoded.first);
  };
  return m_variable->SetMonitorCallback(wrapped_cb);
}

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ENCODED_VARIABLE_T_H_
