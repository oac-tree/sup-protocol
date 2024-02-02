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

#include <gtest/gtest.h>

#include <sup/protocol/exceptions.h>
#include <sup/protocol/process_variable.h>

#include <sup/dto/anyvalue.h>

using namespace sup::protocol;

class TestPV : public ProcessVariable
{
public:
  TestPV(const sup::dto::AnyValue& value, bool available)
    : m_value{value}, m_available{available} {}
  ~TestPV() = default;

  bool IsAvailable() const override { return m_available; }

  sup::dto::AnyValue GetValue(double timeout_sec) const override {
    m_received_timeout = timeout_sec;
    if (m_available)
    {
      return m_value;
    }
    throw VariableUnavailableException("Variable is not available");
  }

  bool SetValue(const sup::dto::AnyValue& value, double timeout_sec) override {
    m_received_timeout = timeout_sec;
    if (m_available)
    {
      m_value = value;
      return true;
    }
    return false;
  }

  bool WaitForAvailable(double timeout_sec) const override {
    m_received_timeout = timeout_sec;
    return false;
  }

  bool SetMonitorCallback(Callback func) override {
    return false;
  }

  sup::dto::AnyValue m_value;
  bool m_available;
  mutable double m_received_timeout;
};

class ProcessVariableTest : public ::testing::Test
{
protected:
  ProcessVariableTest() = default;
  virtual ~ProcessVariableTest() = default;
};

TEST_F(ProcessVariableTest, Available)
{
  TestPV pv{{}, true};
  EXPECT_TRUE(pv.IsAvailable());
  auto readback = pv.GetValue(2.0);
  EXPECT_TRUE(sup::dto::IsEmptyValue(readback));
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  sup::dto::AnyValue val = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  EXPECT_TRUE(pv.SetValue(val, 2.0));
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  readback = GetVariableValue(pv);
  EXPECT_EQ(pv.m_received_timeout, 0.0);
  EXPECT_EQ(readback, val);
  val["flag"] = false;
  EXPECT_TRUE(SetVariableValue(pv, val));
  EXPECT_EQ(pv.m_received_timeout, 0.0);
  EXPECT_EQ(pv.m_value, val);
}

TEST_F(ProcessVariableTest, NotAvailable)
{
  TestPV pv{{}, false};
  EXPECT_FALSE(pv.IsAvailable());
  EXPECT_THROW(pv.GetValue(2.0), VariableUnavailableException);
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  EXPECT_THROW(GetVariableValue(pv), VariableUnavailableException);
  EXPECT_EQ(pv.m_received_timeout, 0.0);
  sup::dto::AnyValue val = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  EXPECT_FALSE(pv.SetValue(val, 2.0));
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  EXPECT_FALSE(SetVariableValue(pv, val));
  EXPECT_EQ(pv.m_received_timeout, 0.0);
}
