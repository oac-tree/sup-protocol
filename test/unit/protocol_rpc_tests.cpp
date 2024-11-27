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

TEST_F(ProtocolRPCTest, CheckRequestFormat)
{
  // Correctly formatted request
  sup::dto::AnyValue correct_request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_TRUE(utils::CheckRequestFormat(correct_request));

  // Correctly formatted request with extra field
  sup::dto::AnyValue correct_request_extra_field = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }},
    { "extra_field", "extra content" }
  }};
  EXPECT_TRUE(utils::CheckRequestFormat(correct_request_extra_field));

  // Missing obsolete timestamp field
  sup::dto::AnyValue request_missing_timestamp = {{
    { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_TRUE(utils::CheckRequestFormat(request_missing_timestamp));

  // Incorrect type of obsolete timestamp field
  sup::dto::AnyValue request_wrong_timestamp_type = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::SignedInteger16Type, 42 }},
    { constants::REQUEST_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_FALSE(utils::CheckRequestFormat(request_wrong_timestamp_type));

  // Missing mandatory payload
  sup::dto::AnyValue request_missing_payload = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }}
  }};
  EXPECT_FALSE(utils::CheckRequestFormat(request_missing_payload));
}

TEST_F(ProtocolRPCTest, CheckReplyFormat)
{
  // Correctly formatted reply
  sup::dto::AnyValue correct_reply = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, "some reason"}
  }};
  EXPECT_TRUE(utils::CheckReplyFormat(correct_reply));

  // Correctly formatted reply with payload
  sup::dto::AnyValue correct_reply_payload = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_TRUE(utils::CheckReplyFormat(correct_reply_payload));

  // Correctly formatted reply with payload and extra field
  sup::dto::AnyValue correct_reply_extra_field = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }},
    { "extra_field", "extra content" }
  }};
  EXPECT_TRUE(utils::CheckReplyFormat(correct_reply_extra_field));

  // Reply with missing mandatory result field
  sup::dto::AnyValue reply_missing_result = {{
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_FALSE(utils::CheckReplyFormat(reply_missing_result));

  // Reply with wrong type for result field
  sup::dto::AnyValue reply_wrong_result_type = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_FALSE(utils::CheckReplyFormat(reply_wrong_result_type));

  // Reply with missing obsolete timestamp field
  sup::dto::AnyValue reply_missing_timestamp = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_TRUE(utils::CheckReplyFormat(reply_missing_timestamp));

  // Reply with wrong type for obsolete timestamp field
  sup::dto::AnyValue reply_wrong_timestamp_type = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::SignedInteger16Type, 42 }},
    { constants::REPLY_REASON, "some reason"},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_FALSE(utils::CheckReplyFormat(reply_wrong_timestamp_type));

  // Reply with missing obsolete reason field
  sup::dto::AnyValue reply_missing_reason = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_TRUE(utils::CheckReplyFormat(reply_missing_reason));

  // Reply with wrong type for obsolete reason field
  sup::dto::AnyValue reply_wrong_reason_type = {{
    { constants::REPLY_RESULT, {sup::dto::UnsignedInteger32Type, 42u }},
    { constants::REPLY_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 42u }},
    { constants::REPLY_REASON, 3.14 },
    { constants::REPLY_PAYLOAD, {sup::dto::BooleanType, true }}
  }};
  EXPECT_FALSE(utils::CheckReplyFormat(reply_wrong_reason_type));
}

TEST_F(ProtocolRPCTest, CreateRPCRequest)
{
  // Request without payload is not allowed and throws
  EXPECT_THROW(utils::CreateRPCRequest({}, PayloadEncoding::kNone), InvalidOperationException);

  {
    // Request with payload and no encoding
    sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    auto request_payload = utils::CreateRPCRequest(payload, PayloadEncoding::kNone);
    EXPECT_EQ(request_payload.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(request_payload.HasField(constants::REQUEST_PAYLOAD));
    auto payload_result = utils::TryExtractRPCPayload(request_payload, constants::REQUEST_PAYLOAD,
                                                      PayloadEncoding::kNone);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    EXPECT_TRUE(utils::CheckRequestFormat(request_payload));
  }
  {
    // Request with payload and base64 encoding
    sup::dto::AnyValue payload = {{
      { "enabled", true },
      { "value", 2.0f }
    }};
    auto request_payload = utils::CreateRPCRequest(payload, PayloadEncoding::kBase64);
    EXPECT_EQ(request_payload.GetTypeName(), constants::REQUEST_TYPE_NAME);
    ASSERT_TRUE(request_payload.HasField(constants::REQUEST_PAYLOAD));
    auto payload_result = utils::TryExtractRPCPayload(request_payload, constants::REQUEST_PAYLOAD,
                                                      PayloadEncoding::kBase64);
    ASSERT_TRUE(payload_result.first);
    auto payload_from_request = payload_result.second;
    EXPECT_EQ(payload_from_request.GetType(), payload.GetType());
    EXPECT_EQ(payload_from_request, payload);
    EXPECT_TRUE(utils::CheckRequestFormat(request_payload));
  }
}

TEST_F(ProtocolRPCTest, CreateRPCReply)
{
  // Reply from Success result without payload
  auto reply_from_result = utils::CreateRPCReply(Success);
  EXPECT_EQ(reply_from_result.GetTypeName(), constants::REPLY_TYPE_NAME);
  ASSERT_TRUE(reply_from_result.HasField(constants::REPLY_RESULT));
  EXPECT_EQ(reply_from_result[constants::REPLY_RESULT].GetType(),
            sup::dto::UnsignedInteger32Type);
  EXPECT_EQ(reply_from_result[constants::REPLY_RESULT].As<unsigned int>(), Success.GetValue());
  EXPECT_FALSE(reply_from_result.HasField(constants::REPLY_PAYLOAD));
  EXPECT_TRUE(utils::CheckReplyFormat(reply_from_result));

  // Reply from NotConnected result without payload
  auto reply_not_connected = utils::CreateRPCReply(NotConnected);
  EXPECT_EQ(reply_not_connected.GetTypeName(), constants::REPLY_TYPE_NAME);
  ASSERT_TRUE(reply_not_connected.HasField(constants::REPLY_RESULT));
  EXPECT_EQ(reply_not_connected[constants::REPLY_RESULT].GetType(),
            sup::dto::UnsignedInteger32Type);
  EXPECT_EQ(reply_not_connected[constants::REPLY_RESULT].As<unsigned int>(),
            NotConnected.GetValue());
  EXPECT_FALSE(reply_not_connected.HasField(constants::REPLY_PAYLOAD));
  EXPECT_TRUE(utils::CheckReplyFormat(reply_not_connected));

  // Reply from ServerNetworkEncodingError result and payload
  sup::dto::AnyValue payload = {{
    { "enabled", true },
    { "value", 2.0f }
  }};
  auto reply_payload =
    utils::CreateRPCReply(ServerNetworkEncodingError, payload, PayloadEncoding::kNone);
  EXPECT_EQ(reply_payload.GetTypeName(), constants::REPLY_TYPE_NAME);
  ASSERT_TRUE(reply_payload.HasField(constants::REPLY_RESULT));
  EXPECT_EQ(reply_payload[constants::REPLY_RESULT].GetType(),
            sup::dto::UnsignedInteger32Type);
  EXPECT_EQ(reply_payload[constants::REPLY_RESULT].As<unsigned int>(),
            ServerNetworkEncodingError.GetValue());
  ASSERT_TRUE(reply_payload.HasField(constants::REPLY_PAYLOAD));
  EXPECT_EQ(reply_payload[constants::REPLY_PAYLOAD].GetType(), payload.GetType());
  EXPECT_EQ(reply_payload[constants::REPLY_PAYLOAD], payload);
  EXPECT_TRUE(utils::CheckReplyFormat(reply_payload));
}

TEST_F(ProtocolRPCTest, IsServiceRequest)
{
  {
    // Empty value is not a service request
    sup::dto::AnyValue request;
    EXPECT_FALSE(utils::IsServiceRequest(request));
  }
  {
    // Empty struct is not a service request
    sup::dto::AnyValue request = sup::dto::EmptyStruct();
    EXPECT_FALSE(utils::IsServiceRequest(request));
  }
  {
    // Only struct with a 'service' member is a service request
    sup::dto::AnyValue request = {
      { constants::SERVICE_REQUEST_PAYLOAD, {sup::dto::StringType, "does_not_matter"}}
    };
    EXPECT_TRUE(utils::IsServiceRequest(request));
  }
  {
    // Struct with a 'service' member is a service request, also when it's not a string
    sup::dto::AnyValue request = {
      { constants::SERVICE_REQUEST_PAYLOAD, {sup::dto::BooleanType, true}}
    };
    EXPECT_TRUE(utils::IsServiceRequest(request));
  }
  {
    // Value created by CreateServiceRequest is a valid service request
    sup::dto::AnyValue request =
      utils::CreateServiceRequest({sup::dto::StringType, "does_not_matter"},
                                  PayloadEncoding::kBase64 );
    EXPECT_TRUE(utils::IsServiceRequest(request));
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
    // Correctly formatted server status reply with payload
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
    // Struct without required field is not valid
    sup::dto::AnyValue reply = {
      { "wrong_field_name", {sup::dto::UnsignedInteger16Type, 0}}
    };
    EXPECT_FALSE(utils::CheckServiceReplyFormat(reply));
  }
  {
    // Struct with required field of wrong type is not valid
    sup::dto::AnyValue reply = {
      { constants::SERVICE_REPLY_RESULT, {sup::dto::UnsignedInteger16Type, 0}}
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
    // Empty payload throws
    sup::dto::AnyValue payload{ sup::dto::StringType, "service_payload" };
    auto service_request = utils::CreateServiceRequest(payload, PayloadEncoding::kNone);
    EXPECT_TRUE(utils::IsServiceRequest(service_request));
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
  }
  {
    // Reply without payload by providing empty payload
    sup::dto::AnyValue payload;
    auto service_reply =
      utils::CreateServiceReply(sup::protocol::Success, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(service_reply));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_RESULT));
    EXPECT_FALSE(service_reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
  }
  {
    // Reply with payload
    sup::dto::AnyValue payload{ sup::dto::Float64Type, 3.14 };
    auto service_reply =
      utils::CreateServiceReply(sup::protocol::Success, payload, PayloadEncoding::kBase64);
    EXPECT_TRUE(utils::CheckServiceReplyFormat(service_reply));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_RESULT));
    EXPECT_TRUE(service_reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
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

ProtocolRPCTest::ProtocolRPCTest() = default;

ProtocolRPCTest::~ProtocolRPCTest() = default;
