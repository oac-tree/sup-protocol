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

#include <sup/dto/anyvalue.h>
#include <sup/protocol/log_any_functor_decorator.h>

#include <vector>
#include <utility>

using namespace sup::protocol;

class EchoFunctor : public sup::dto::AnyFunctor
{
public:
  EchoFunctor() = default;
  ~EchoFunctor() = default;

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override
  {
    return input;
  }
};

class LogAnyFunctorDecoratorTest : public ::testing::Test
{
protected:
  LogAnyFunctorDecoratorTest();
  virtual ~LogAnyFunctorDecoratorTest();

  EchoFunctor m_functor;
  LogAnyFunctorDecorator::LogFunction m_log_function;
  std::vector<std::pair<LogAnyFunctorDecorator::PacketDirection, sup::dto::AnyValue>> m_log_entries;
};

TEST_F(LogAnyFunctorDecoratorTest, LogEntries)
{
  using PacketDirection = LogAnyFunctorDecorator::PacketDirection;
  LogAnyFunctorDecorator decorator{m_functor, m_log_function};
  {
    // Test with empty payload
    sup::dto::AnyValue request{};
    auto reply = decorator(request);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 2);
    EXPECT_EQ(m_log_entries[0].first, PacketDirection::kInput);
    EXPECT_EQ(m_log_entries[0].second, request);
    EXPECT_EQ(m_log_entries[1].first, PacketDirection::kOutput);
    EXPECT_EQ(m_log_entries[1].second, reply);
  }
  {
    // Test with scalar payload
    auto request = sup::dto::AnyValue(sup::dto::UnsignedInteger16Type, 46);
    auto reply = decorator(request);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 4);
    EXPECT_EQ(m_log_entries[2].first, PacketDirection::kInput);
    EXPECT_EQ(m_log_entries[2].second, request);
    EXPECT_EQ(m_log_entries[3].first, PacketDirection::kOutput);
    EXPECT_EQ(m_log_entries[3].second, reply);
  }
}

LogAnyFunctorDecoratorTest::LogAnyFunctorDecoratorTest()
  : m_functor{}
  , m_log_function{}
{
  m_log_function = [this](const sup::dto::AnyValue& value,
                          LogAnyFunctorDecorator::PacketDirection direction) {
    m_log_entries.emplace_back(direction, value);
  };
}

LogAnyFunctorDecoratorTest::~LogAnyFunctorDecoratorTest() = default;
