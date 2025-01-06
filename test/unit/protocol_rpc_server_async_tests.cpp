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

#include "test_protocol.h"

#include <sup/protocol/protocol_rpc_server.h>

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/exceptions.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>

#include <thread>

using namespace sup::protocol;

class ProtocolRPCServerAsyncTest : public ::testing::Test
{
protected:
  ProtocolRPCServerAsyncTest();
  virtual ~ProtocolRPCServerAsyncTest();

  Protocol& GetTestProtocol();

  test::TestProtocol m_test_protocol;
};

TEST_F(ProtocolRPCServerAsyncTest, ScalarPayload)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }},
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}}
  }};
  auto reply = server(request);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Poll until ready and get reply
  ASSERT_TRUE(test::PollUntilReady(server, id, 1.0));
  auto get_reply_req = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kNone);
  reply = server(get_reply_req);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);

  // Check protocol
  auto last_input = m_test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), sup::dto::UnsignedInteger8Type);
  EXPECT_EQ(last_input.As<sup::dto::uint8>(), 1);
}

TEST_F(ProtocolRPCServerAsyncTest, ExtraFieldInRequest)
{
  ProtocolRPCServer server{GetTestProtocol()};

  // Extra fields are ignored in the request structure
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }},
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}},
    { "extra_field", {sup::dto::Float32Type, 3.25f }}
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Poll until ready and get reply
  ASSERT_TRUE(test::PollUntilReady(server, id, 1.0));
  auto get_reply_req = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kBase64);
  reply = server(get_reply_req);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);

  // Check protocol
  auto last_input = m_test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), sup::dto::UnsignedInteger8Type);
  EXPECT_EQ(last_input.As<sup::dto::uint8>(), 1);
}

TEST_F(ProtocolRPCServerAsyncTest, ProtocolThrows)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::THROW_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload },
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}},
  }};
  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Poll until ready and get reply
  ASSERT_TRUE(test::PollUntilReady(server, id, 1.0));
  auto get_reply_req = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kNone);
  reply = server(get_reply_req);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, ServerProtocolException);

  // Check protocol
  auto last_input = m_test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);
}

TEST_F(ProtocolRPCServerAsyncTest, Protocol_UnsupportedPayloadEncoding)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload{ sup::dto::StringType, "does_not_matter"};
  sup::dto::AnyValue request = utils::CreateAsyncRPCRequest(payload, PayloadEncoding::kBase64);
  request[constants::ENCODING_FIELD_NAME].ConvertFrom(42);

  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, ServerUnsupportedPayloadEncodingError);
}

TEST_F(ProtocolRPCServerAsyncTest, RequestProtocolResult)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::REQUESTED_STATUS_FIELD, {sup::dto::UnsignedInteger32Type, 42 }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload },
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}}
  }};

  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Poll until ready and get reply
  ASSERT_TRUE(test::PollUntilReady(server, id, 1.0));
  auto get_reply_req = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kBase64);
  reply = server(get_reply_req);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, ProtocolResult{42u});
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  // Check protocol
  auto last_input = m_test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);
}

TEST_F(ProtocolRPCServerAsyncTest, EchoPayload)
{
  ProtocolRPCServer server{GetTestProtocol()};

  sup::dto::AnyValue payload = {{
    { test::REQUESTED_STATUS_FIELD, {sup::dto::UnsignedInteger32Type, 65 }},
    { test::ECHO_FIELD, {sup::dto::BooleanType, true }}
  }};
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, payload },
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}}
  }};

  auto reply = server(request);
  EXPECT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Poll until ready and get reply
  ASSERT_TRUE(test::PollUntilReady(server, id, 1.0));
  auto get_reply_req = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kBase64);
  reply = server(get_reply_req);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, ProtocolResult{65u});

  // Check returned payload
  auto extract_payload = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kBase64);
  ASSERT_TRUE(extract_payload.first);
  auto reply_payload = extract_payload.second;
  EXPECT_EQ(reply_payload.GetType(), payload.GetType());
  EXPECT_EQ(reply_payload, payload);

  // Check protocol
  auto last_input = m_test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), payload.GetType());
  EXPECT_EQ(last_input, payload);
}

TEST_F(ProtocolRPCServerAsyncTest, ExpiredRequest)
{
  // Test the clean up of expired requests
  ProtocolRPCServerConfig config{0.01};
  ProtocolRPCServer server{GetTestProtocol(), config};

  sup::dto::AnyValue request = {{
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }},
    { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0}}
  }};
  auto reply = server(request);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  auto result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, Success);
  auto id = test::ExtractRequestId(reply);
  ASSERT_NE(id, 0);

  // Wait enough time for the request to have been expired:
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Poll and check that request was already cleaned up (id is no longer valid):
  auto poll_request = utils::CreateAsyncRPCPoll(id, PayloadEncoding::kBase64);
  reply = server(poll_request);
  result = utils::TryExtractProtocolResult(reply);
  EXPECT_TRUE(result.first);
  EXPECT_EQ(result.second, InvalidRequestIdentifierError);
}

ProtocolRPCServerAsyncTest::ProtocolRPCServerAsyncTest()
  : m_test_protocol{}
{}

ProtocolRPCServerAsyncTest::~ProtocolRPCServerAsyncTest() = default;

Protocol& ProtocolRPCServerAsyncTest::GetTestProtocol()
{
  return m_test_protocol;
}
