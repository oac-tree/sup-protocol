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

#ifndef SUP_CONFIG_TEST_PROCESS_VARIABLE_H_
#define SUP_CONFIG_TEST_PROCESS_VARIABLE_H_

#include <sup/protocol/process_variable.h>

#include <sup/dto/anyvalue.h>

#include <mutex>

namespace sup
{
namespace protocol
{
namespace test
{
class TestProcessVariable : public ProcessVariable
{
public:
  TestProcessVariable(const sup::dto::AnyValue& val, bool available);
  ~TestProcessVariable();

  bool IsAvailable() const override;
  std::pair<bool, sup::dto::AnyValue> GetValue(double timeout_sec) const override;
  bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) override;
  bool WaitForAvailable(double timeout_sec) const override;
  bool SetMonitorCallback(Callback func) override;

  sup::dto::AnyValue m_value;
  bool m_available;
  mutable double m_received_timeout;
private:
  Callback m_cb;
  mutable std::mutex m_mtx;
};

}  // namespace test

}  // namespace protocol

}  // namespace sup

#endif  // SUP_CONFIG_TEST_PROCESS_VARIABLE_H_
