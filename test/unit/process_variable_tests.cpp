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

#include "test_process_variable.h"

#include <sup/protocol/encoded_variables.h>
#include <sup/protocol/exceptions.h>
#include <sup/protocol/process_variable.h>

#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>
#include <memory>

using namespace sup::protocol;

class ProcessVariableTest : public ::testing::Test
{
protected:
  ProcessVariableTest() = default;
  virtual ~ProcessVariableTest() = default;

  std::unique_ptr<ProcessVariable> GetTestProcessVariable(const sup::dto::AnyValue& val,
                                                          bool available = true)
  {
    return std::make_unique<test::TestProcessVariable>(val, available);
  }
};

TEST_F(ProcessVariableTest, Available)
{
  test::TestProcessVariable pv{{}, true};
  sup::dto::AnyValue val = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  EXPECT_TRUE(pv.IsAvailable());
  EXPECT_FALSE(WaitForVariableValue(pv, val, 0.1));
  auto info = pv.GetValue(2.0);
  EXPECT_TRUE(info.first);
  auto readback = info.second;
  EXPECT_TRUE(sup::dto::IsEmptyValue(readback));
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  EXPECT_TRUE(pv.SetValue(val, 2.0));
  EXPECT_EQ(pv.m_received_timeout, 2.0);
  EXPECT_TRUE(WaitForVariableValue(pv, val, 1.0));
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
  test::TestProcessVariable pv{{}, false};
  EXPECT_FALSE(pv.IsAvailable());
  auto info = pv.GetValue(2.0);
  EXPECT_FALSE(info.first);
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

TEST_F(ProcessVariableTest, Base64PVGetSet)
{
  sup::dto::AnyValue empty_val{};
  auto test_pv = GetTestProcessVariable(empty_val);
  ASSERT_TRUE(test_pv);
  auto pv_impl = test_pv.get();
  Base64ProcessVariable base64_pv{std::move(test_pv)};
  // Variable is unavailable if it does not contain a valid value, i.e. one that can be
  // properly decoded
  EXPECT_FALSE(base64_pv.IsAvailable());
  EXPECT_FALSE(base64_pv.WaitForAvailable(0.1));

  // Set empty value
  EXPECT_TRUE(base64_pv.SetValue(empty_val, 0.1));
  auto encoded = pv_impl->GetValue(0.1);
  EXPECT_TRUE(encoded.first);
  EXPECT_TRUE(ValidateBase64AnyValue(encoded.second));
  auto decoded = base64_pv.GetValue(0.1);
  EXPECT_TRUE(decoded.first);
  EXPECT_EQ(decoded.second, empty_val);

  // Set structured value
  sup::dto::AnyValue new_value = {{
    {"flag", {sup::dto::BooleanType, true}},
    {"setpoint", {sup::dto::Float64Type, 42.0}}
  }};
  EXPECT_TRUE(base64_pv.SetValue(new_value, 0.1));
  encoded = pv_impl->GetValue(0.1);
  EXPECT_TRUE(encoded.first);
  EXPECT_TRUE(ValidateBase64AnyValue(encoded.second));
  decoded = base64_pv.GetValue(0.1);
  EXPECT_TRUE(decoded.first);
  EXPECT_EQ(decoded.second, new_value);

  // Back to empty value
  EXPECT_TRUE(base64_pv.SetValue(empty_val, 0.1));
  encoded = pv_impl->GetValue(0.1);
  EXPECT_TRUE(encoded.first);
  EXPECT_TRUE(ValidateBase64AnyValue(encoded.second));
  decoded = base64_pv.GetValue(0.1);
  EXPECT_TRUE(decoded.first);
  EXPECT_EQ(decoded.second, empty_val);
}

TEST_F(ProcessVariableTest, Base64PVCallback)
{
  sup::dto::AnyValue empty_val{};
  auto test_pv = GetTestProcessVariable(empty_val);
  ASSERT_TRUE(test_pv);
  auto pv_impl = test_pv.get();
  Base64ProcessVariable base64_pv{std::move(test_pv)};
  EXPECT_FALSE(base64_pv.IsAvailable());
  EXPECT_FALSE(base64_pv.WaitForAvailable(0.1));

  // Set callback
  sup::dto::AnyValue pv_update;
  auto cb = [&pv_update](const sup::dto::AnyValue& val, bool available) {
    if (available) {
      pv_update = val;
    }
  };
  EXPECT_TRUE(base64_pv.SetMonitorCallback(cb));

  // Set structured value
  sup::dto::AnyValue new_value = {{
    {"flag", {sup::dto::BooleanType, true}},
    {"setpoint", {sup::dto::Float64Type, 42.0}}
  }};
  EXPECT_TRUE(base64_pv.SetValue(new_value, 0.1));
  EXPECT_TRUE(base64_pv.IsAvailable());
  EXPECT_TRUE(base64_pv.WaitForAvailable(0.1));
  auto encoded = pv_impl->GetValue(0.1);
  EXPECT_TRUE(encoded.first);
  EXPECT_TRUE(ValidateBase64AnyValue(encoded.second));
  auto decoded = base64_pv.GetValue(0.1);
  EXPECT_TRUE(decoded.first);
  EXPECT_EQ(decoded.second, new_value);
  EXPECT_EQ(pv_update, new_value);

  // Set empty value
  EXPECT_TRUE(base64_pv.SetValue(empty_val, 0.1));
  encoded = pv_impl->GetValue(0.1);
  EXPECT_TRUE(encoded.first);
  EXPECT_TRUE(ValidateBase64AnyValue(encoded.second));
  decoded = base64_pv.GetValue(0.1);
  EXPECT_TRUE(decoded.first);
  EXPECT_EQ(decoded.second, empty_val);
  EXPECT_EQ(pv_update, empty_val);

  // Set incompatible value to underlying pv
  sup::dto::AnyValue wrong_av = encoded.second;
  EXPECT_TRUE(wrong_av.HasField(kValueField));
  wrong_av[kValueField] = "AB";  // Missing == padding for it to be a valid base64 string
  EXPECT_TRUE(pv_impl->SetValue(wrong_av, 0.1));
  // Callback did not update pv_update, since decoding failed; pv becomes unavailable too
  EXPECT_FALSE(base64_pv.IsAvailable());
  EXPECT_FALSE(base64_pv.WaitForAvailable(0.1));
  EXPECT_EQ(pv_update, empty_val);
  decoded = base64_pv.GetValue(0.1);
  EXPECT_FALSE(decoded.first);
  EXPECT_EQ(decoded.second, empty_val);
}
