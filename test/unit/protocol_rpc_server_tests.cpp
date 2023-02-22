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
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#include "test_protocol.h"

#include <sup/protocol/protocol_rpc_server.h>

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/rpc_exceptions.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class ProtocolRPCServerTest : public ::testing::Test
{
protected:
  ProtocolRPCServerTest();
  virtual ~ProtocolRPCServerTest();

  std::unique_ptr<Protocol> GetTestProtocol();

  test::TestProtocol* m_test_protocol;
};

TEST_F(ProtocolRPCServerTest, Construction)
{
  EXPECT_THROW(ProtocolRPCServer null_server{std::unique_ptr<Protocol>{}},
               NullDependencyException);
  EXPECT_NO_THROW(ProtocolRPCServer server{GetTestProtocol()});
}

TEST_F(ProtocolRPCServerTest, EmptyRequest)
{
  ProtocolRPCServer server{GetTestProtocol()};
  sup::dto::AnyValue request;
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportDecodingError.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));
}

TEST_F(ProtocolRPCServerTest, BadRequest)
{
  ProtocolRPCServer server{GetTestProtocol()};

  // Request contains no timestamp field
  sup::dto::AnyValue request_no_timestamp = {{
    { constants::REPLY_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }}
  }};
  auto reply_1 = server(request_no_timestamp);
  EXPECT_TRUE(utils::CheckReplyFormat(reply_1));
  EXPECT_EQ(reply_1[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportDecodingError.GetValue());
  EXPECT_FALSE(reply_1.HasField(constants::REPLY_PAYLOAD));

  // Request contains no payload field
  sup::dto::AnyValue request_no_payload = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }}
  }};
  auto reply_2 = server(request_no_payload);
  EXPECT_TRUE(utils::CheckReplyFormat(reply_2));
  EXPECT_EQ(reply_2[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportDecodingError.GetValue());
  EXPECT_FALSE(reply_2.HasField(constants::REPLY_PAYLOAD));

  // Request contains timestamp field of wrong type
  sup::dto::AnyValue request_wrong_timestamp_type = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger32Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }}
  }};
  auto reply_3 = server(request_wrong_timestamp_type);
  EXPECT_TRUE(utils::CheckReplyFormat(reply_3));
  EXPECT_EQ(reply_3[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportDecodingError.GetValue());
  EXPECT_FALSE(reply_3.HasField(constants::REPLY_PAYLOAD));
}

TEST_F(ProtocolRPCServerTest, ScalarPayload)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }}
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(), Success.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  auto last_input = m_test_protocol->GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), sup::dto::UnsignedInteger8Type);
  EXPECT_EQ(last_input.As<sup::dto::uint8>(), 1);
}

TEST_F(ProtocolRPCServerTest, ExtraFieldInRequest)
{
  ProtocolRPCServer server{GetTestProtocol()};

  // Extra fields are ignored in the request structure
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }},
    { "extra_field", {sup::dto::Float32Type, 3.25f }}
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(), Success.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  auto last_input = m_test_protocol->GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), sup::dto::UnsignedInteger8Type);
  EXPECT_EQ(last_input.As<sup::dto::uint8>(), 1);
}

TEST_F(ProtocolRPCServerTest, ProtocolThrows)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::THROW_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload }
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportEncodingError.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  auto last_input = m_test_protocol->GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);
}

TEST_F(ProtocolRPCServerTest, RequestProtocolResult)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::REQUESTED_STATUS_FIELD, {sup::dto::UnsignedInteger32Type, 42 }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload }
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(), 42u);
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  auto last_input = m_test_protocol->GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);
}

TEST_F(ProtocolRPCServerTest, EchoPayload)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::REQUESTED_STATUS_FIELD, {sup::dto::UnsignedInteger32Type, 65 }},
    { test::ECHO_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload }
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(), 65u);

  auto last_input = m_test_protocol->GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);

  EXPECT_TRUE(reply.HasField(constants::REPLY_PAYLOAD));
  auto reply_payload = reply[constants::REPLY_PAYLOAD];
  EXPECT_EQ(reply_payload.GetType(), payload.GetType());
  EXPECT_EQ(reply_payload, payload);
}

TEST_F(ProtocolRPCServerTest, ServiceThrows)
{
  ProtocolRPCServer server{GetTestProtocol()};
  m_test_protocol->SetThrowForServiceRequest(true);

  sup::dto::AnyValue payload{ sup::dto::StringType, "does_not_matter"};
  sup::dto::AnyValue request = utils::CreateServiceRequest(payload);
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckServiceReplyFormat(reply));
  EXPECT_EQ(reply[constants::SERVICE_REPLY_RESULT].As<unsigned int>(),
            ServerTransportEncodingError.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));
}

ProtocolRPCServerTest::ProtocolRPCServerTest()
  : m_test_protocol{nullptr}
{}

ProtocolRPCServerTest::~ProtocolRPCServerTest() = default;

std::unique_ptr<Protocol> ProtocolRPCServerTest::GetTestProtocol()
{
  m_test_protocol = new test::TestProtocol{};
  return std::unique_ptr<Protocol>(m_test_protocol);
}