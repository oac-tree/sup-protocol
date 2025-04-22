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

#include "test_process_variable.h"

#include <sup/protocol/factory/process_variable_utils.h>

#include <gtest/gtest.h>

#include <future>
#include <memory>


using namespace sup::protocol;

class ProcessVariableUtilsTest : public ::testing::Test
{
protected:
  ProcessVariableUtilsTest() = default;
  virtual ~ProcessVariableUtilsTest() = default;

  std::unique_ptr<ProcessVariable> GetTestProcessVariable(const sup::dto::AnyValue& val,
                                                          bool available = true)
  {
    return std::make_unique<test::TestProcessVariable>(val, available);
  }
};

TEST_F(ProcessVariableUtilsTest, TryFetchVariable)
{
  {
    // Variable with available value:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}}
    }};
    test::TestProcessVariable pv{val, true};
    sup::dto::AnyValue read_back{};
    EXPECT_NO_THROW(utils::TryFetchVariable(pv, read_back));
    EXPECT_EQ(read_back, val);
  }
  {
    // Variable with unavailable value:
    sup::dto::AnyValue val = {{
      {"flag", {sup::dto::BooleanType, true}}
    }};
    test::TestProcessVariable pv{val, false};
    sup::dto::AnyValue read_back{};
    EXPECT_NO_THROW(utils::TryFetchVariable(pv, read_back));
    EXPECT_TRUE(sup::dto::IsEmptyValue(read_back));
  }
}

TEST_F(ProcessVariableUtilsTest, BusyWaitForValue)
{
  {
    // Variable will have the expected value within the timeout:
    std::promise<void> ready;
    auto ready_future = ready.get_future();
    sup::dto::AnyValue initial_val = {{
      {"flag", {sup::dto::BooleanType, true}}
    }};
    sup::dto::AnyValue expected_val = {{
      {"flag", {sup::dto::BooleanType, false}}
    }};
    test::TestProcessVariable pv{initial_val, true};
    auto var_setter = [&ready, &pv, expected_val]() {
      ready.set_value();
      pv.SetValue(expected_val, 0.0);
    };
    auto task = std::async(std::launch::async, var_setter);
    ready_future.get();
    EXPECT_TRUE(utils::BusyWaitForValue(pv, expected_val, 0.5));
  }
  {
    // Variable does not get the expected value within the timeout:
    sup::dto::AnyValue initial_val = {{
      {"flag", {sup::dto::BooleanType, true}}
    }};
    sup::dto::AnyValue expected_val = {{
      {"flag", {sup::dto::BooleanType, false}}
    }};
    test::TestProcessVariable pv{initial_val, true};
    EXPECT_FALSE(utils::BusyWaitForValue(pv, expected_val, 0.02));
  }
}
