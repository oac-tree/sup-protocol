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

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/exceptions.h>

#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class ProtocolRPCTest : public ::testing::Test
{
protected:
  ProtocolRPCTest();
  virtual ~ProtocolRPCTest();
};

TEST_F(ProtocolRPCTest, SupportedEncoding)
{
  // No encoding
  EXPECT_TRUE(utils::IsSupportedPayloadEncoding(PayloadEncoding::kNone));
  // Base64
  EXPECT_TRUE(utils::IsSupportedPayloadEncoding(PayloadEncoding::kBase64));
  // Next enum integer is not supported
  EXPECT_FALSE(utils::IsSupportedPayloadEncoding(utils::EncodingFromInteger(2)));
  // Higher enum integer is not supported
  EXPECT_FALSE(utils::IsSupportedPayloadEncoding(utils::EncodingFromInteger(42)));
}

TEST_F(ProtocolRPCTest, CheckRequestFormat)
{
  {
    // Minimal request
    sup::dto::AnyValue request = {{
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
    }};
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request with correct timestamp
    sup::dto::AnyValue request = {{
      { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
    }};
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request with correct encoding field
    sup::dto::AnyValue request = {{
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 87 }},
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
    }};
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request with correct async field
    sup::dto::AnyValue request = {{
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 42u }},
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
    }};
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request with extra unknown field (ignored)
    sup::dto::AnyValue request = {{
      { "extra_field", "extra content" },
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
    }};
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request with wrong type of timestamp field
    sup::dto::AnyValue request = {{
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }},
      { constants::REQUEST_TIMESTAMP, {sup::dto::SignedInteger16Type, 42 }}
    }};
    EXPECT_FALSE(utils::CheckRequestFormat(request));
  }
  {
    // Request with wrong type of encoding field
    sup::dto::AnyValue request = {{
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }},
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger16Type, 42 }}
    }};
    EXPECT_FALSE(utils::CheckRequestFormat(request));
  }
  {
    // Request with wrong type of async field
    sup::dto::AnyValue request = {{
      { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }},
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::SignedInteger16Type, 42 }}
    }};
    EXPECT_FALSE(utils::CheckRequestFormat(request));
  }
  {
    // Request with missing payload field
    sup::dto::AnyValue request = {{
      { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 87 }},
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_FALSE(utils::CheckRequestFormat(request));
  }
  {
    // Request generated with helper function
    sup::dto::AnyValue payload = {{
      { "flag", {sup::dto::BooleanType, true }},
      { "set_point", {sup::dto::Float64Type, 3.14 }}
    }};
    auto request = utils::CreateRPCRequest(payload, PayloadEncoding::kNone);
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
  {
    // Request generated with async helper function
    sup::dto::AnyValue payload = {{
      { "flag", {sup::dto::BooleanType, true }},
      { "set_point", {sup::dto::Float64Type, 3.14 }}
    }};
    auto request = utils::CreateAsyncRPCRequest(payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckRequestFormat(request));
  }
}

TEST_F(ProtocolRPCTest, CheckReplyFormat)
{
  {
    // Minimal reply
    sup::dto::AnyValue reply = {{
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with timestamp
    sup::dto::AnyValue reply = {{
      { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with encoding
    sup::dto::AnyValue reply = {{
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 0 }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with reason field
    sup::dto::AnyValue reply = {{
      { constants::REPLY_REASON, {sup::dto::StringType, "some reason" }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with async field
    sup::dto::AnyValue reply = {{
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0 }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with extra unknown field (ignored)
    sup::dto::AnyValue reply = {{
      { "unknown_field", true },
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with wrong type of timestamp field
    sup::dto::AnyValue reply = {{
      { constants::REPLY_TIMESTAMP, {sup::dto::SignedInteger16Type, 42 }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_FALSE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with wrong type of encoding field
    sup::dto::AnyValue reply = {{
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger16Type, 42 }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_FALSE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with wrong type of reason field
    sup::dto::AnyValue reply = {{
      { constants::REPLY_REASON, {sup::dto::Float32Type, 3.14 }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_FALSE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with wrong type of async field
    sup::dto::AnyValue reply = {{
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger64Type, 1729u }},
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }}
    }};
    EXPECT_FALSE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply with missing result field
    sup::dto::AnyValue reply = {{
      { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 0 }},
      { constants::REPLY_REASON, {sup::dto::StringType, "some reason" }},
      { constants::ASYNC_COMMAND_FIELD_NAME, {sup::dto::UnsignedInteger32Type, 0 }},
    }};
    EXPECT_FALSE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply generated with helper function, no payload
    auto reply = utils::CreateRPCReply(Success);
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Reply generated with helper function, with payload
    sup::dto::AnyValue payload = {{
      { "flag", {sup::dto::BooleanType, true }},
      { "set_point", {sup::dto::Float64Type, 3.14 }}
    }};
    auto reply = utils::CreateRPCReply(NotConnected, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Async reply generated with helper function, no payload
    auto reply = utils::CreateAsyncRPCReply(Success, AsyncCommand::kInitialRequest);
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
  {
    // Async reply generated with helper function, with payload
    sup::dto::AnyValue payload = {{
      { "flag", {sup::dto::BooleanType, true }},
      { "set_point", {sup::dto::Float64Type, 3.14 }}
    }};
    auto reply = utils::CreateAsyncRPCReply(NotConnected, payload, PayloadEncoding::kBase64,
                                            AsyncCommand::kGetReply);
    EXPECT_TRUE(utils::CheckReplyFormat(reply));
  }
}

TEST_F(ProtocolRPCTest, CreateRPCRequest)
{
  // Request without payload is not allowed and throws
  EXPECT_THROW(utils::CreateRPCRequest({}, PayloadEncoding::kNone), InvalidOperationException);

  {
    // Request with payload and no encoding
    sup::dto::AnyValue payload{ sup::dto::UnsignedInteger8Type, 5u };
    auto request = utils::CreateRPCRequest(payload, PayloadEncoding::kNone);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check payload
    auto& payload_field = request[constants::REQUEST_PAYLOAD];
    EXPECT_EQ(payload_field.GetType(), payload.GetType());
    auto payload_result = utils::TryExtractRPCRequestPayload(request, PayloadEncoding::kNone);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_FALSE(async_info.first);
  }
  {
    // Request with payload and base64 encoding
    sup::dto::AnyValue payload{ sup::dto::UnsignedInteger8Type, 5u };
    auto request = utils::CreateRPCRequest(payload, PayloadEncoding::kBase64);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check payload
    auto& payload_field = request[constants::REQUEST_PAYLOAD];
    EXPECT_EQ(payload_field.GetType(), sup::dto::StringType);
    auto payload_result = utils::TryExtractRPCRequestPayload(request, PayloadEncoding::kBase64);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_FALSE(async_info.first);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCRequest)
{
  // Request without payload is not allowed and throws
  EXPECT_THROW(utils::CreateAsyncRPCRequest({}, PayloadEncoding::kNone), InvalidOperationException);

  {
    // Request with payload and no encoding
    sup::dto::AnyValue payload{ sup::dto::UnsignedInteger8Type, 5u };
    auto request = utils::CreateAsyncRPCRequest(payload, PayloadEncoding::kNone);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check payload
    auto& payload_field = request[constants::REQUEST_PAYLOAD];
    EXPECT_EQ(payload_field.GetType(), payload.GetType());
    auto payload_result = utils::TryExtractRPCRequestPayload(request, PayloadEncoding::kNone);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInitialRequest);
  }
  {
    // Request with payload and base64 encoding
    sup::dto::AnyValue payload{ sup::dto::UnsignedInteger8Type, 5u };
    auto request = utils::CreateAsyncRPCRequest(payload, PayloadEncoding::kBase64);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check payload
    auto& payload_field = request[constants::REQUEST_PAYLOAD];
    EXPECT_EQ(payload_field.GetType(), sup::dto::StringType);
    auto payload_result = utils::TryExtractRPCRequestPayload(request, PayloadEncoding::kBase64);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInitialRequest);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCPoll)
{
  {
    // Poll request without encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCPoll(id, PayloadEncoding::kNone);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kNone);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kPoll);
  }
  {
    // Poll request with encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCPoll(id, PayloadEncoding::kBase64);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kBase64);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kPoll);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCGetReply)
{
  {
    // GetReply request without encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kNone);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kNone);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kGetReply);
  }
  {
    // GetReply request with encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCGetReply(id, PayloadEncoding::kBase64);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kBase64);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kGetReply);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCInvalidate)
{
  {
    // Invalidate request without encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCInvalidate(id, PayloadEncoding::kNone);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kNone);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInvalidate);
  }
  {
    // Invalidate request with encoding
    sup::dto::uint64 id = 42u;
    auto request = utils::CreateAsyncRPCInvalidate(id, PayloadEncoding::kBase64);
    EXPECT_EQ(request.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(utils::CheckRequestFormat(request));
    // check poll id
    auto poll_id = utils::TryExtractRequestId(request, PayloadEncoding::kBase64);
    EXPECT_TRUE(poll_id.first);
    EXPECT_EQ(poll_id.second, id);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(request);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check async info
    auto async_info = utils::GetAsyncInfo(request);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInvalidate);
  }
}

TEST_F(ProtocolRPCTest, CreateRPCReply)
{
  {
    // Reply Success result without payload
    auto reply = utils::CreateRPCReply(Success);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_FALSE(payload_info.first);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_FALSE(async_info.first);
  }
  {
    // Reply NotConnected result without payload
    auto reply = utils::CreateRPCReply(NotConnected);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, NotConnected);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_FALSE(payload_info.first);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_FALSE(async_info.first);
  }
  {
    // Reply ServerNetworkEncodingError result with payload, no encoding
    const sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    auto reply =
      utils::CreateRPCReply(ServerNetworkEncodingError, payload, PayloadEncoding::kNone);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, ServerNetworkEncodingError);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(payload_info.first);
    EXPECT_EQ(payload_info.second, payload);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_FALSE(async_info.first);
  }
  {
    // Reply ClientProtocolEncodingError result with payload and encoding
    const sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    auto reply =
      utils::CreateRPCReply(ClientProtocolEncodingError, payload, PayloadEncoding::kBase64);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, ClientProtocolEncodingError);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(payload_info.first);
    EXPECT_EQ(payload_info.second, payload);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_FALSE(async_info.first);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCReply)
{
  {
    // Reply Success result without payload
    AsyncCommand command = AsyncCommand::kInitialRequest;
    auto reply = utils::CreateAsyncRPCReply(Success, command);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_FALSE(payload_info.first);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, command);
  }
  {
    // Reply NotConnected result without payload
    AsyncCommand command = AsyncCommand::kPoll;
    auto reply = utils::CreateAsyncRPCReply(NotConnected, command);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, NotConnected);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_FALSE(payload_info.first);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, command);
  }
  {
    // Reply ServerNetworkEncodingError result with payload, no encoding
    const sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    AsyncCommand command = AsyncCommand::kGetReply;
    auto reply = utils::CreateAsyncRPCReply(ServerNetworkEncodingError, payload,
                                            PayloadEncoding::kNone, command);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, ServerNetworkEncodingError);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(payload_info.first);
    EXPECT_EQ(payload_info.second, payload);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, command);
  }
  {
    // Reply ClientProtocolEncodingError result with payload and encoding
    const sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    AsyncCommand command = AsyncCommand::kInvalidate;
    auto reply = utils::CreateAsyncRPCReply(ClientProtocolEncodingError, payload,
                                            PayloadEncoding::kBase64, command);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, ClientProtocolEncodingError);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(payload_info.first);
    EXPECT_EQ(payload_info.second, payload);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, command);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCNewRequestReply)
{
  {
    // Reply to new async request without encoding
    sup::dto::uint64 id{42u};
    auto reply = utils::CreateAsyncRPCNewRequestReply(id, PayloadEncoding::kNone);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(payload_info.first);
    // check id in payload
    auto id_info = utils::TryExtractReplyId(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(id_info.first);
    EXPECT_EQ(id_info.second, id);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInitialRequest);
  }
  {
    // Reply to new async request with base64 encoding
    sup::dto::uint64 id{42u};
    auto reply = utils::CreateAsyncRPCNewRequestReply(id, PayloadEncoding::kBase64);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(payload_info.first);
    // check id in payload
    auto id_info = utils::TryExtractReplyId(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(id_info.first);
    EXPECT_EQ(id_info.second, id);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kInitialRequest);
  }
}

TEST_F(ProtocolRPCTest, CreateAsyncRPCPollReply)
{
  {
    // Reply to poll with ready, no encoding
    const bool is_ready = true;
    auto reply = utils::CreateAsyncRPCPollReply(is_ready, PayloadEncoding::kNone);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kNone);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(payload_info.first);
    // check id in payload
    auto ready_info = utils::TryExtractReadyStatus(reply, PayloadEncoding::kNone);
    EXPECT_TRUE(ready_info.first);
    EXPECT_EQ(ready_info.second, is_ready);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kPoll);
  }
  {
    // Reply to poll with not ready, base64 encoding
    const bool is_ready = false;
    auto reply = utils::CreateAsyncRPCPollReply(is_ready, PayloadEncoding::kBase64);
    EXPECT_EQ(reply.GetTypeName(), constants::REPLY_TYPE_NAME);
    ASSERT_TRUE(utils::CheckReplyFormat(reply));
    // check result field
    auto result_info = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result_info.first);
    EXPECT_EQ(result_info.second, Success);
    // check encoding
    auto encoding_result = utils::TryGetPacketEncoding(reply);
    EXPECT_TRUE(encoding_result.first);
    EXPECT_EQ(encoding_result.second, PayloadEncoding::kBase64);
    // check payload field
    auto payload_info = utils::TryExtractRPCReplyPayload(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(payload_info.first);
    // check id in payload
    auto ready_info = utils::TryExtractReadyStatus(reply, PayloadEncoding::kBase64);
    EXPECT_TRUE(ready_info.first);
    EXPECT_EQ(ready_info.second, is_ready);
    // check async info
    auto async_info = utils::GetAsyncInfo(reply);
    EXPECT_TRUE(async_info.first);
    EXPECT_EQ(async_info.second, AsyncCommand::kPoll);
  }
}

TEST_F(ProtocolRPCTest, CheckServiceRequest)
{
  {
    // Empty value is not a service request
    sup::dto::AnyValue request;
    EXPECT_FALSE(utils::CheckServiceRequest(request));
  }
  {
    // Empty struct is not a service request
    sup::dto::AnyValue request = sup::dto::EmptyStruct();
    EXPECT_FALSE(utils::CheckServiceRequest(request));
  }
  {
    // Valid service request, no encoding
    sup::dto::AnyValue request = {
      { constants::SERVICE_REQUEST_PAYLOAD, {sup::dto::StringType, "does_not_matter"}}
    };
    EXPECT_TRUE(utils::CheckServiceRequest(request));
  }
  {
    // valid service request, base64 encoding
    sup::dto::AnyValue request = {
      { constants::ENCODING_FIELD_NAME, { sup::dto::SignedInteger32Type, 1 }},
      { constants::SERVICE_REQUEST_PAYLOAD, {sup::dto::BooleanType, true}}
    };
    EXPECT_TRUE(utils::CheckServiceRequest(request));
  }
  {
    // Value created by CreateServiceRequest without encoding is a valid service request
    sup::dto::AnyValue request =
      utils::CreateServiceRequest({sup::dto::StringType, "does_not_matter"},
                                  PayloadEncoding::kNone );
    EXPECT_TRUE(utils::CheckServiceRequest(request));
  }
  {
    // Value created by CreateServiceRequest with base64 encoding is a valid service request
    sup::dto::AnyValue request =
      utils::CreateServiceRequest({sup::dto::StringType, "does_not_matter"},
                                  PayloadEncoding::kBase64 );
    EXPECT_TRUE(utils::CheckServiceRequest(request));
  }
}

TEST_F(ProtocolRPCTest, CheckServiceReplyFormat)
{
  {
    // Correctly formatted server status reply without payload
    sup::dto::AnyValue reply = utils::CreateServiceReply(sup::protocol::Success);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Correctly formatted server status reply with payload and base64 encoding
    sup::dto::AnyValue payload = {sup::dto::BooleanType, true};
    sup::dto::AnyValue reply =
      utils::CreateServiceReply(sup::protocol::Success, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Empty reply is not valid
    sup::dto::AnyValue reply;
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Empty struct is not valid
    sup::dto::AnyValue reply = sup::dto::EmptyStruct();
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Manually constructed valid reply
    sup::dto::AnyValue reply = {
      { constants::SERVICE_REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 0}},
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 1}}
    };
    EXPECT_TRUE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Manually constructed valid reply without encoding field
    sup::dto::AnyValue reply = {
      { constants::SERVICE_REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 0}}
    };
    EXPECT_TRUE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Reply without result field is invalid
    sup::dto::AnyValue reply = {
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 1}}
    };
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Reply with wrong type of result field is invalid
    sup::dto::AnyValue reply = {
      { constants::SERVICE_REPLY_RESULT, {sup::dto::SignedInteger8Type, 0}},
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, 1}}
    };
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Reply with wrong type of encoding field is invalid
    sup::dto::AnyValue reply = {
      { constants::SERVICE_REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 0}},
      { constants::ENCODING_FIELD_NAME, {sup::dto::UnsignedInteger64Type, 1}}
    };
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
}

TEST_F(ProtocolRPCTest, CreateServiceRequest)
{
  {
    // Empty payload throws
    sup::dto::AnyValue payload;
    EXPECT_THROW(utils::CreateServiceRequest(payload, PayloadEncoding::kNone),
                 InvalidOperationException);
  }
  {
    // Non-empty payload succeeds
    sup::dto::AnyValue payload{ sup::dto::StringType, "service_payload" };
    auto service_request = utils::CreateServiceRequest(payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceRequest(service_request));
    // check encoding
    auto encoding_info = utils::TryGetPacketEncoding(service_request);
    EXPECT_TRUE(encoding_info.first);
    EXPECT_EQ(encoding_info.second, PayloadEncoding::kBase64);
  }
}

TEST_F(ProtocolRPCTest, CreateServiceReply)
{
  {
    // Reply without payload
    auto service_reply = utils::CreateServiceReply(sup::protocol::Success);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(service_reply));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_RESULT));
    EXPECT_FALSE(service_reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
    // check encoding
    auto encoding_info = utils::TryGetPacketEncoding(service_reply);
    EXPECT_TRUE(encoding_info.first);
    EXPECT_EQ(encoding_info.second, PayloadEncoding::kNone);
  }
  {
    // Reply without payload by providing empty payload
    sup::dto::AnyValue payload;
    auto service_reply =
      utils::CreateServiceReply(sup::protocol::Success, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(service_reply));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_RESULT));
    EXPECT_FALSE(service_reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
    // check encoding (without payload, there is no encoding information, so it defaults to none)
    auto encoding_info = utils::TryGetPacketEncoding(service_reply);
    EXPECT_TRUE(encoding_info.first);
    EXPECT_EQ(encoding_info.second, PayloadEncoding::kNone);
  }
  {
    // Reply with payload
    sup::dto::AnyValue payload{ sup::dto::Float64Type, 3.14 };
    auto service_reply =
      utils::CreateServiceReply(sup::protocol::Success, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(service_reply));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_RESULT));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
    // check encoding
    auto encoding_info = utils::TryGetPacketEncoding(service_reply);
    EXPECT_TRUE(encoding_info.first);
    EXPECT_EQ(encoding_info.second, PayloadEncoding::kBase64);
  }
}

TEST_F(ProtocolRPCTest, IsApplicationProtocolRequestPayload)
{
  {
    // Empty payload is not valid
    sup::dto::AnyValue payload;
    EXPECT_FALSE(utils::IsApplicationProtocolRequestPayload(payload));
  }
  {
    // Non-string payload is not valid
    sup::dto::AnyValue payload{ sup::dto::BooleanType, true };
    EXPECT_FALSE(utils::IsApplicationProtocolRequestPayload(payload));
  }
  {
    // Wrong string payload is not valid
    sup::dto::AnyValue payload{ sup::dto::StringType, "not_an_application_protocol_request" };
    EXPECT_FALSE(utils::IsApplicationProtocolRequestPayload(payload));
  }
  {
    // Only correct string payload is valid
    sup::dto::AnyValue payload{ sup::dto::StringType, constants::APPLICATION_PROTOCOL_INFO_REQUEST };
    EXPECT_TRUE(utils::IsApplicationProtocolRequestPayload(payload));
  }
}

TEST_F(ProtocolRPCTest, CheckApplicationProtocolReplyPayload)
{
  const std::string APPLICATION_TYPE = "test_application";
  const std::string APPLICATION_VERSION = "1.0";
  {
    // Correct reply
    sup::dto::AnyValue payload;
    EXPECT_EQ(utils::HandleApplicationProtocolInfo(
      payload, APPLICATION_TYPE, APPLICATION_VERSION), sup::protocol::Success);
    EXPECT_TRUE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
  {
    // Correct reply with extra field
    sup::dto::AnyValue payload;
    ASSERT_EQ(utils::HandleApplicationProtocolInfo(
      payload, APPLICATION_TYPE, APPLICATION_VERSION), sup::protocol::Success);
    EXPECT_NO_THROW(payload.AddMember("extra_field", {sup::dto::BooleanType, true}));
    EXPECT_TRUE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
  {
    // Reply with missing application protocol type field
    sup::dto::AnyValue payload = {
      { constants::APPLICATION_PROTOCOL_VERSION, {sup::dto::StringType, APPLICATION_VERSION}}
    };
    EXPECT_FALSE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
  {
    // Reply with missing application protocol version field
    sup::dto::AnyValue payload = {
      { constants::APPLICATION_PROTOCOL_TYPE, {sup::dto::StringType, APPLICATION_TYPE}}
    };
    EXPECT_FALSE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
  {
    // Reply with wrong type of application protocol type field
    sup::dto::AnyValue payload = {
      { constants::APPLICATION_PROTOCOL_TYPE, {sup::dto::BooleanType, true}},
      { constants::APPLICATION_PROTOCOL_VERSION, {sup::dto::StringType, APPLICATION_VERSION}}
    };
    EXPECT_FALSE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
  {
    // Reply with wrong type of application protocol version field
    sup::dto::AnyValue payload = {
      { constants::APPLICATION_PROTOCOL_TYPE, {sup::dto::StringType, APPLICATION_TYPE}},
      { constants::APPLICATION_PROTOCOL_VERSION, {sup::dto::BooleanType, true}}
    };
    EXPECT_FALSE(utils::CheckApplicationProtocolReplyPayload(payload));
  }
}

TEST_F(ProtocolRPCTest, GetApplicationProtocolInfo)
{
  test::TestProtocol protocol;
  {
    // Default use
    auto protocol_info = utils::GetApplicationProtocolInfo(protocol);
    EXPECT_EQ(protocol_info.m_application_type, test::TEST_PROTOCOL_TYPE);
    EXPECT_EQ(protocol_info.m_application_version, test::TEST_PROTOCOL_VERSION);
  }
  {
    // Protocol does not return success
    protocol.SetFailForServiceRequest(true);
    auto protocol_info = utils::GetApplicationProtocolInfo(protocol);
    EXPECT_TRUE(protocol_info.m_application_type.empty());
    EXPECT_TRUE(protocol_info.m_application_version.empty());
  }
}

TEST_F(ProtocolRPCTest, HandleApplicationProtocolInfo)
{
  const std::string APPLICATION_TYPE = "test_application";
  const std::string APPLICATION_VERSION = "1.0";
  {
    // Empty output (default use)
    sup::dto::AnyValue output;
    EXPECT_EQ(utils::HandleApplicationProtocolInfo(
      output, APPLICATION_TYPE, APPLICATION_VERSION), sup::protocol::Success);
    EXPECT_TRUE(utils::CheckApplicationProtocolReplyPayload(output));
  }
  {
    // Wrong output type gives error status
    sup::dto::AnyValue output{ sup::dto::StringType, "does_not_matter"};
    EXPECT_EQ(utils::HandleApplicationProtocolInfo(
      output, APPLICATION_TYPE, APPLICATION_VERSION), sup::protocol::ServerProtocolEncodingError);
    EXPECT_FALSE(utils::CheckApplicationProtocolReplyPayload(output));
  }
}

TEST_F(ProtocolRPCTest, TryExtractProtocolResult)
{
  {
    // Successful extraction of protocol result
    sup::dto::AnyValue reply = {
      { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 0}}
    };
    auto result = utils::TryExtractProtocolResult(reply);
    EXPECT_TRUE(result.first);
    EXPECT_EQ(result.second, ProtocolResult{0});
  }
  {
    // Missing result field
    sup::dto::AnyValue reply = {
      { "wrong_field_name", {sup::dto::UnsignedInteger32Type, 0}}
    };
    auto result = utils::TryExtractProtocolResult(reply);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, ClientTransportDecodingError);
  }
  {
    // Wrong type of result field
    sup::dto::AnyValue reply = {
      { constants::REPLY_RESULT, {sup::dto::Float32Type, 1.0}}
    };
    auto result = utils::TryExtractProtocolResult(reply);
    EXPECT_FALSE(result.first);
    EXPECT_EQ(result.second, ClientTransportDecodingError);
  }
}

TEST_F(ProtocolRPCTest, TryExtractReadyStatus)
{
  {
    // Successful extraction of ready status
    sup::dto::AnyValue payload = {
      { constants::ASYNC_READY_FIELD_NAME, {sup::dto::BooleanType, true}}
    };
    auto packet = utils::CreateRPCReply(Success, payload, PayloadEncoding::kBase64);
    auto ready = utils::TryExtractReadyStatus(packet, PayloadEncoding::kBase64);
    EXPECT_TRUE(ready.first);
    EXPECT_TRUE(ready.second);
  }
  {
    // Missing ready status
    sup::dto::AnyValue payload = {
      { "wrong_field_name", {sup::dto::BooleanType, true}}
    };
    auto packet = utils::CreateRPCReply(Success, payload, PayloadEncoding::kBase64);
    auto ready = utils::TryExtractReadyStatus(packet, PayloadEncoding::kBase64);
    EXPECT_FALSE(ready.first);
    EXPECT_FALSE(ready.second);
  }
  {
    // Wrong type of ready status
    sup::dto::AnyValue payload = {
      { constants::ASYNC_READY_FIELD_NAME, {sup::dto::SignedInteger16Type, 16}}
    };
    auto packet = utils::CreateRPCReply(Success, payload, PayloadEncoding::kBase64);
    auto ready = utils::TryExtractReadyStatus(packet, PayloadEncoding::kBase64);
    EXPECT_FALSE(ready.first);
    EXPECT_FALSE(ready.second);
  }
}

TEST_F(ProtocolRPCTest, TryGetPacketEncoding)
{
  {
    // Successful extraction of packet encoding
    auto encoding_int = static_cast<sup::dto::int32>(PayloadEncoding::kBase64);
    sup::dto::AnyValue packet = {
      { constants::ENCODING_FIELD_NAME, {sup::dto::SignedInteger32Type, encoding_int}}
    };
    auto encoding = utils::TryGetPacketEncoding(packet);
    EXPECT_TRUE(encoding.first);
    EXPECT_EQ(encoding.second, PayloadEncoding::kBase64);
  }
  {
    // Missing packet encoding indicates no encoding
    auto encoding_int = static_cast<sup::dto::int32>(PayloadEncoding::kBase64);
    sup::dto::AnyValue packet = {
      { "wrong_field_name", {sup::dto::SignedInteger32Type, encoding_int}}
    };
    auto encoding = utils::TryGetPacketEncoding(packet);
    EXPECT_TRUE(encoding.first);
    EXPECT_EQ(encoding.second, PayloadEncoding::kNone);
  }
  {
    // Wrong type of packet encoding
    auto encoding_int = static_cast<sup::dto::int32>(PayloadEncoding::kBase64);
    sup::dto::AnyValue packet = {
      { constants::ENCODING_FIELD_NAME, {sup::dto::UnsignedInteger32Type, encoding_int}}
    };
    auto encoding = utils::TryGetPacketEncoding(packet);
    EXPECT_FALSE(encoding.first);
    EXPECT_EQ(encoding.second, PayloadEncoding::kNone);
  }
}

ProtocolRPCTest::ProtocolRPCTest() = default;

ProtocolRPCTest::~ProtocolRPCTest() = default;
