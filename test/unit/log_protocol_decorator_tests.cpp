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
#include <sup/protocol/log_protocol_decorator.h>

#include <vector>
#include <utility>

using namespace sup::protocol;

class EchoProtocol : public Protocol
{
public:
  EchoProtocol() = default;
  ~EchoProtocol() = default;

  ProtocolResult Invoke(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override
  {
    output = input;
    return Success;
  }
  ProtocolResult Service(const sup::dto::AnyValue& input, sup::dto::AnyValue& output) override
  {
    output = input;
    return Success;
  }
};

class LogProtocolDecoratorTest : public ::testing::Test
{
protected:
  LogProtocolDecoratorTest();
  virtual ~LogProtocolDecoratorTest();

  EchoProtocol m_protocol;
  LogProtocolDecorator::LogFunction m_log_function;
  std::vector<std::pair<std::string, sup::dto::AnyValue>> m_log_entries;
};

TEST_F(LogProtocolDecoratorTest, InvokeEntries)
{
  LogProtocolDecorator decorator{m_protocol, m_log_function};
  {
    // Test with empty payload
    sup::dto::AnyValue request{};
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Invoke(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 2);
    EXPECT_EQ(m_log_entries[0].first, kLogProtocolRequestTitle);
    EXPECT_EQ(m_log_entries[0].second, request);
    EXPECT_EQ(m_log_entries[1].first, kLogProtocolReplyTitle);
    EXPECT_EQ(m_log_entries[1].second, reply);
  }
  {
    // Test with scalar payload
    auto request = sup::dto::AnyValue(sup::dto::UnsignedInteger16Type, 46);
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Invoke(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 4);
    EXPECT_EQ(m_log_entries[2].first, kLogProtocolRequestTitle);
    EXPECT_EQ(m_log_entries[2].second, request);
    EXPECT_EQ(m_log_entries[3].first, kLogProtocolReplyTitle);
    EXPECT_EQ(m_log_entries[3].second, reply);
  }
}

TEST_F(LogProtocolDecoratorTest, ServiceEntries)
{
  LogProtocolDecorator decorator{m_protocol, m_log_function};
  {
    // Test with empty payload
    sup::dto::AnyValue request{};
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Service(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 2);
    EXPECT_EQ(m_log_entries[0].first, kLogProtocolServiceRequestTitle);
    EXPECT_EQ(m_log_entries[0].second, request);
    EXPECT_EQ(m_log_entries[1].first, kLogProtocolServiceReplyTitle);
    EXPECT_EQ(m_log_entries[1].second, reply);
  }
  {
    // Test with scalar payload
    auto request = sup::dto::AnyValue(sup::dto::UnsignedInteger16Type, 46);
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Service(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_entries.size(), 4);
    EXPECT_EQ(m_log_entries[2].first, kLogProtocolServiceRequestTitle);
    EXPECT_EQ(m_log_entries[2].second, request);
    EXPECT_EQ(m_log_entries[3].first, kLogProtocolServiceReplyTitle);
    EXPECT_EQ(m_log_entries[3].second, reply);
  }
}

LogProtocolDecoratorTest::LogProtocolDecoratorTest()
  : m_protocol{}
  , m_log_function{}
{
  m_log_function = [this](const sup::dto::AnyValue& value, const std::string& message) {
    m_log_entries.emplace_back(message, value);
  };
}

LogProtocolDecoratorTest::~LogProtocolDecoratorTest() = default;
