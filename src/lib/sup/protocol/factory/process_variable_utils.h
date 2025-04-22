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

#ifndef SUP_PROTOCOL_PROCESS_VARIABLE_UTILS_H_
#define SUP_PROTOCOL_PROCESS_VARIABLE_UTILS_H_

#include <sup/protocol/process_variable.h>

#include <sup/dto/anyvalue.h>

namespace sup
{
namespace protocol
{
namespace utils
{
/**
 * @brief Try to fetch the value of a ProcessVariable. On failure, `output` is left unchanged.
 *
 * @param var ProcessVariable to read.
 * @param output Output AnyValue that will hold the ProcessVariable's value on success.
 */
void TryFetchVariable(const ProcessVariable& var, sup::dto::AnyValue& output);

/**
 * @brief Wait for a ProcessVariable to have an expected value in a busy loop and with timeout.
 *
 * @param var ProcessVariable to read.
 * @param expected_value Expected value to wait for.
 * @param timeout_sec Timeout in seconds.
 *
 * @return True if the ProcessVariable reached the expected value within the timeout. False
 * otherwise.
 */
bool BusyWaitForValue(const ProcessVariable& var, const sup::dto::AnyValue& expected_value,
                      double timeout_sec);

}  // namespace utils

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROCESS_VARIABLE_UTILS_H_
