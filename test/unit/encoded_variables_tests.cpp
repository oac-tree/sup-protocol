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

#include <gtest/gtest.h>

#include "test_process_variable.h"

#include <sup/protocol/encoded_variables.h>
#include <sup/protocol/exceptions.h>
#include <memory>

using namespace sup::protocol;

class EncodedVariablesTest : public ::testing::Test
{
protected:
  EncodedVariablesTest();
  virtual ~EncodedVariablesTest();

  std::unique_ptr<ProcessVariable> GetTestProcessVariable(const sup::dto::AnyValue& val,
                                                          bool available = true)
  {
    return std::make_unique<test::TestProcessVariable>(val, available);
  }

};

TEST_F(EncodedVariablesTest, ValidVariable)
{
  // Basic API
  sup::dto::AnyValue val = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  auto test_variable = GetTestProcessVariable({}, true);
  Base64ProcessVariable encoded_variable{std::move(test_variable)};
  EXPECT_TRUE(encoded_variable.SetValue(val, 0.0));
  auto read_back = encoded_variable.GetValue(0.0);
  EXPECT_TRUE(read_back.first);
  EXPECT_EQ(read_back.second, val);
  EXPECT_TRUE(encoded_variable.WaitForAvailable(0.0));
  // Test callback
  sup::dto::AnyValue monitor{};
  auto cb = [&monitor](const sup::dto::AnyValue& value, bool available)
  {
    if (available)
    {
      monitor = value;
    }
  };
  EXPECT_TRUE(encoded_variable.SetMonitorCallback(cb));
  EXPECT_TRUE(sup::dto::IsEmptyValue(monitor));
  sup::dto::AnyValue update = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  EXPECT_TRUE(encoded_variable.SetValue(update, 0.0));
  EXPECT_EQ(monitor, update);
}

TEST_F(EncodedVariablesTest, FailureCases)
{
  {
    // Trying to construct an encoded process variable with a nullptr:
    EXPECT_THROW(Base64ProcessVariable{nullptr}, InvalidOperationException);
  }
  {
    // Encoded process variable whose underlying value cannot be decoded:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}}
    }};
    auto test_variable = GetTestProcessVariable(val, true);
    Base64ProcessVariable encoded_variable{std::move(test_variable)};
    auto read_back = encoded_variable.GetValue(0.0);
    EXPECT_FALSE(read_back.first);
    EXPECT_TRUE(sup::dto::IsEmptyValue(read_back.second));
    EXPECT_FALSE(encoded_variable.WaitForAvailable(0.0));
  }
}

EncodedVariablesTest::EncodedVariablesTest() = default;

EncodedVariablesTest::~EncodedVariablesTest() = default;
