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

#include <tuple>
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
  LogProtocolDecorator::LogInputFunction m_log_input_function;
  LogProtocolDecorator::LogOutputFunction m_log_output_function;
  std::vector<std::pair<sup::dto::AnyValue,
                        sup::protocol::LogProtocolDecorator::PacketType>> m_log_input_entries;
  std::vector<std::tuple<ProtocolResult, sup::dto::AnyValue,
                         sup::protocol::LogProtocolDecorator::PacketType>> m_log_output_entries;
};

TEST_F(LogProtocolDecoratorTest, InvokeEntries)
{
  using PacketType = LogProtocolDecorator::PacketType;
  LogProtocolDecorator decorator{m_protocol, m_log_input_function, m_log_output_function};
  {
    // Test with empty payload
    sup::dto::AnyValue request{};
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Invoke(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_input_entries.size(), 1);
    ASSERT_EQ(m_log_output_entries.size(), 1);
    EXPECT_EQ(m_log_input_entries[0].first, request);
    EXPECT_EQ(m_log_input_entries[0].second, PacketType::kNormal);
    EXPECT_EQ(std::get<0>(m_log_output_entries[0]), Success);
    EXPECT_EQ(std::get<1>(m_log_output_entries[0]), reply);
    EXPECT_EQ(std::get<2>(m_log_output_entries[0]), PacketType::kNormal);
  }
  {
    // Test with scalar payload
    auto request = sup::dto::AnyValue(sup::dto::UnsignedInteger16Type, 46);
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Invoke(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_input_entries.size(), 2);
    ASSERT_EQ(m_log_output_entries.size(), 2);
    EXPECT_EQ(m_log_input_entries[1].first, request);
    EXPECT_EQ(m_log_input_entries[1].second, PacketType::kNormal);
    EXPECT_EQ(std::get<0>(m_log_output_entries[1]), Success);
    EXPECT_EQ(std::get<1>(m_log_output_entries[1]), reply);
    EXPECT_EQ(std::get<2>(m_log_output_entries[1]), PacketType::kNormal);
  }
}

TEST_F(LogProtocolDecoratorTest, ServiceEntries)
{
  using PacketType = LogProtocolDecorator::PacketType;
  LogProtocolDecorator decorator{m_protocol, m_log_input_function, m_log_output_function};
  {
    // Test with empty payload
    sup::dto::AnyValue request{};
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Service(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_input_entries.size(), 1);
    ASSERT_EQ(m_log_output_entries.size(), 1);
    EXPECT_EQ(m_log_input_entries[0].first, request);
    EXPECT_EQ(m_log_input_entries[0].second, PacketType::kService);
    EXPECT_EQ(std::get<0>(m_log_output_entries[0]), Success);
    EXPECT_EQ(std::get<1>(m_log_output_entries[0]), reply);
    EXPECT_EQ(std::get<2>(m_log_output_entries[0]), PacketType::kService);
  }
  {
    // Test with scalar payload
    auto request = sup::dto::AnyValue(sup::dto::UnsignedInteger16Type, 46);
    sup::dto::AnyValue reply;
    EXPECT_EQ(decorator.Service(request, reply), Success);
    EXPECT_EQ(reply, request);
    ASSERT_EQ(m_log_input_entries.size(), 2);
    ASSERT_EQ(m_log_output_entries.size(), 2);
    EXPECT_EQ(m_log_input_entries[1].first, request);
    EXPECT_EQ(m_log_input_entries[1].second, PacketType::kService);
    EXPECT_EQ(std::get<0>(m_log_output_entries[1]), Success);
    EXPECT_EQ(std::get<1>(m_log_output_entries[1]), reply);
    EXPECT_EQ(std::get<2>(m_log_output_entries[1]), PacketType::kService);
  }
}

LogProtocolDecoratorTest::LogProtocolDecoratorTest()
  : m_protocol{}
  , m_log_input_function{}
  , m_log_output_function{}
{
  m_log_input_function =
    [this](const sup::dto::AnyValue& value, LogProtocolDecorator::PacketType type) {
      m_log_input_entries.emplace_back(value, type);
  };
  m_log_output_function =
    [this](ProtocolResult result, const sup::dto::AnyValue& value,
           LogProtocolDecorator::PacketType type) {
      m_log_output_entries.emplace_back(result, value, type);
  };
}

LogProtocolDecoratorTest::~LogProtocolDecoratorTest() = default;
