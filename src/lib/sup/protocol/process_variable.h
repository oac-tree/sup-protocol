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

#ifndef SUP_PROTOCOL_PROCESS_VARIABLE_H_
#define SUP_PROTOCOL_PROCESS_VARIABLE_H_

#include <sup/protocol/protocol.h>

#include <functional>
#include <utility>

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
  ProcessVariable() = default;
  ProcessVariable(const ProcessVariable&) = delete;
  ProcessVariable(ProcessVariable&&) = delete;
  ProcessVariable& operator=(const ProcessVariable&) = delete;
  ProcessVariable& operator=(ProcessVariable&&) = delete;
  /**
   * @brief Callback type for updates in value or availability.
   */
  using Callback = std::function<void(const sup::dto::AnyValue&, bool)>;

  virtual ~ProcessVariable();

  /**
   * @brief Check if variable is available. For network variables, this means they are connected
   * and have a valid value.
   *
   * @return True if variable is available, false otherwise.
   */
  virtual bool IsAvailable() const = 0;

  /**
   * @brief Retrieve the variable's value.
   *
   * @param timeout_sec Maximum timeout in seconds to retrieve the value, i.e. to wait for the
   * variable to become available.
   *
   * @return Boolean indicating possible success and variable's value, which is only meaningful
   * if successful.
   */
  virtual std::pair<bool, sup::dto::AnyValue> GetValue(double timeout_sec) const = 0;

  /**
   * @brief Write a value to the variable.
   *
   * @param value Value to be written.
   * @param timeout_sec Maximum timeout in seconds to set the value, i.e. to wait for the
   * variable to become available.
   *
   * @return True if successful, false otherwise.
   */
  virtual bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) = 0;

  /**
   * @brief This method waits for the variable to be available with a timeout.
   *
   * @param timeout_sec Timeout in seconds to wait for the variable to be available.
   *
   * @return True if the variable was available within the timeout period.
   */
  virtual bool WaitForAvailable(double timeout_sec) const = 0;

  /**
   * @brief Set a callback for value/availability updates.
   *
   * @details Some variable implementations may not support callbacks and will always return false
   * on calling this member function.
   *
   * @param func Callable target to use for updates.
   *
   * @return True if callback was successfully set.
   */
  virtual bool SetMonitorCallback(Callback func) = 0;
};

/**
 * @brief Retrieve a ProcessVariable's value without timeout.
 *
 * @param var ProcessVariable to retrieve value from.
 *
 * @return ProcessVariable's value.
 * @throws VariableUnavailableException when the variable is not available within the timeout.
 */
sup::dto::AnyValue GetVariableValue(const ProcessVariable& var);

/**
 * @brief Set a ProcessVariable's value without timeout.
 *
 * @param var ProcessVariable to set.
 * @param value Value to be written.
 *
 * @return True if successful, false otherwise.
 */
bool SetVariableValue(ProcessVariable& var, const sup::dto::AnyValue& value);

/**
 * @brief Wait for a ProcessVariable's value to have a given value within a specified timeout.
 *
 * @details This function should not be used with an expected empty value, as in that case the
 * function may return true even when it could not fetch the ProcessVariable's value.
 * When the ProcessVariable supports callbacks, they will be used. Otherwise, the wait will be a
 * busy wait. Note also that this function will remove any existing callbacks defined previously.
 *
 * @param var ProcessVariable to monitor.
 * @param value Value to wait for.
 * @param timeout_sec Timeout in seconds to wait for.
 *
 * @return True if the ProcessVariable gets the specified value within the given timeout.
 */
bool WaitForVariableValue(ProcessVariable& var, const sup::dto::AnyValue& value,
                          double timeout_sec);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROCESS_VARIABLE_H_
