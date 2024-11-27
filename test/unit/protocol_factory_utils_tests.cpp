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

#include "test_functor.h"
#include "test_protocol.h"

#include <sup/protocol/exceptions.h>
#include <sup/protocol/protocol_factory_utils.h>
#include <sup/protocol/protocol_rpc.h>

#include <sup/dto/any_functor.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class TestRPCServer : public RPCServerInterface
{
public:
  TestRPCServer(sup::dto::AnyFunctor& protocol_server, TestRPCServer*& handle)
    : m_protocol_server{protocol_server}
  {
    handle = this;
  }
  ~TestRPCServer() = default;

  sup::dto::AnyValue Call(const sup::dto::AnyValue& input) {
    return m_protocol_server(input);
  }
private:
  sup::dto::AnyFunctor& m_protocol_server;
};

class ProtocolFactoryUtilsTest : public ::testing::Test
{
protected:
  ProtocolFactoryUtilsTest() = default;
  virtual ~ProtocolFactoryUtilsTest() = default;

  std::unique_ptr<RPCServerInterface> CreateTestRPCServer(
    sup::dto::AnyFunctor& protocol_server, TestRPCServer*& handle)
  {
    return std::unique_ptr<RPCServerInterface>(new TestRPCServer(protocol_server, handle));
  }

  std::unique_ptr<sup::dto::AnyFunctor> CreateTestFunctor(test::TestFunctor*& handle)
  {
    return std::unique_ptr<sup::dto::AnyFunctor>{new test::TestFunctor(handle)};
  }
};

TEST_F(ProtocolFactoryUtilsTest, CreateServerStack)
{
  test::TestProtocol test_protocol{};
  TestRPCServer* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestRPCServer(protocol_server, server_handle);
  };
  auto server_stack = CreateRPCServerStack(factory_func, test_protocol);
  ASSERT_NE(server_handle, nullptr);
  sup::dto::AnyValue payload = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  auto request = utils::CreateRPCRequest(payload, PayloadEncoding::kNone);
  EXPECT_NO_THROW(server_handle->Call(request));
}

TEST_F(ProtocolFactoryUtilsTest, ServerStackEmptyRequest)
{
  test::TestProtocol test_protocol{};
  TestRPCServer* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestRPCServer(protocol_server, server_handle);
  };
  auto server_stack = CreateRPCServerStack(factory_func, test_protocol);
  ASSERT_NE(server_handle, nullptr);
  sup::dto::AnyValue request;
  auto reply = server_handle->Call(request);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(),
            ServerTransportDecodingError.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));
}

TEST_F(ProtocolFactoryUtilsTest, ServerStackScalarPayload)
{
  test::TestProtocol test_protocol{};
  TestRPCServer* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestRPCServer(protocol_server, server_handle);
  };
  auto server_stack = CreateRPCServerStack(factory_func, test_protocol);
  ASSERT_NE(server_handle, nullptr);
  sup::dto::AnyValue request = {{
    { constants::REQUEST_TIMESTAMP, {sup::dto::UnsignedInteger64Type, 54321u }},
    { constants::REQUEST_PAYLOAD, {sup::dto::UnsignedInteger8Type, 1 }}
  }};
  auto reply = server_handle->Call(request);
  ASSERT_TRUE(utils::CheckReplyFormat(reply));
  EXPECT_EQ(reply[constants::REPLY_RESULT].As<unsigned int>(), Success.GetValue());
  EXPECT_FALSE(reply.HasField(constants::REPLY_PAYLOAD));

  auto last_input = test_protocol.GetLastInput();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_input));
  EXPECT_EQ(last_input.GetType(), sup::dto::UnsignedInteger8Type);
  EXPECT_EQ(last_input.As<sup::dto::uint8>(), 1);
}

TEST_F(ProtocolFactoryUtilsTest, ServerStackServiceRequest)
{
  test::TestProtocol test_protocol{};
  TestRPCServer* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestRPCServer(protocol_server, server_handle);
  };
  auto server_stack = CreateRPCServerStack(factory_func, test_protocol);
  ASSERT_NE(server_handle, nullptr);
  sup::dto::AnyValue payload{sup::dto::StringType, constants::APPLICATION_PROTOCOL_INFO_REQUEST};
  auto request = utils::CreateServiceRequest(payload, PayloadEncoding::kNone);
  auto reply = server_handle->Call(request);
  ASSERT_TRUE(utils::CheckServiceReplyFormat(reply));
  EXPECT_EQ(reply[constants::SERVICE_REPLY_RESULT].As<unsigned int>(), Success.GetValue());
  ASSERT_TRUE(reply.HasField(constants::SERVICE_REPLY_PAYLOAD));
  sup::dto::AnyValue expected_payload = {
    { constants::APPLICATION_PROTOCOL_TYPE, {sup::dto::StringType, test::TEST_PROTOCOL_TYPE} },
    { constants::APPLICATION_PROTOCOL_VERSION, {sup::dto::StringType, test::TEST_PROTOCOL_VERSION} }
  };
  EXPECT_EQ(reply[constants::SERVICE_REPLY_PAYLOAD], expected_payload);
}

TEST_F(ProtocolFactoryUtilsTest, CreateClientStack)
{
  test::TestFunctor* functor_handle = nullptr;
  auto factory_func = [this, &functor_handle]() {
    return CreateTestFunctor(functor_handle);
  };
  auto client_stack = CreateRPCClientStack(factory_func, PayloadEncoding::kNone);
  ASSERT_NE(functor_handle, nullptr);
  sup::dto::AnyValue input = {{
    {"flag", {sup::dto::BooleanType, true}}
  }};
  sup::dto::AnyValue output{};
  EXPECT_NO_THROW(client_stack->Invoke(input, output));
  EXPECT_TRUE(utils::CheckRequestFormat(functor_handle->GetLastRequest()));
}

TEST_F(ProtocolFactoryUtilsTest, ClientStackEmptyPayload)
{
  test::TestFunctor* functor_handle = nullptr;
  auto factory_func = [this, &functor_handle]() {
    return CreateTestFunctor(functor_handle);
  };
  auto client_stack = CreateRPCClientStack(factory_func, PayloadEncoding::kNone);
  ASSERT_NE(functor_handle, nullptr);
  sup::dto::AnyValue input;
  sup::dto::AnyValue output{};
  EXPECT_EQ(client_stack->Invoke(input, output), ClientTransportEncodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
  EXPECT_TRUE(sup::dto::IsEmptyValue(functor_handle->GetLastRequest()));
}

TEST_F(ProtocolFactoryUtilsTest, ClientStackScalarPayload)
{
  test::TestFunctor* functor_handle = nullptr;
  auto factory_func = [this, &functor_handle]() {
    return CreateTestFunctor(functor_handle);
  };
  auto client_stack = CreateRPCClientStack(factory_func, PayloadEncoding::kBase64);
  ASSERT_NE(functor_handle, nullptr);
  sup::dto::AnyValue input{sup::dto::SignedInteger32Type, 42};
  sup::dto::AnyValue output{};
  EXPECT_EQ(client_stack->Invoke(input, output), Success);
  EXPECT_FALSE(sup::dto::IsEmptyValue(output));
  EXPECT_EQ(input, output);
  auto last_request = functor_handle->GetLastRequest();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_request));
  EXPECT_TRUE(utils::CheckRequestFormat(last_request));
  EXPECT_EQ(last_request.GetTypeName(), constants::REQUEST_TYPE_NAME);
  EXPECT_TRUE(last_request.HasField(constants::REQUEST_PAYLOAD));
  auto payload_result = utils::TryExtractRPCPayload(last_request, constants::REQUEST_PAYLOAD,
                                                    PayloadEncoding::kBase64);
  ASSERT_TRUE(payload_result.first);
  auto payload = payload_result.second;
  EXPECT_EQ(payload.GetType(), sup::dto::SignedInteger32Type);
  EXPECT_EQ(payload, input);
}

TEST_F(ProtocolFactoryUtilsTest, ClientStackServiceRequest)
{
  test::TestFunctor* functor_handle = nullptr;
  auto factory_func = [this, &functor_handle]() {
    return CreateTestFunctor(functor_handle);
  };
  auto client_stack = CreateRPCClientStack(factory_func, PayloadEncoding::kBase64);
  ASSERT_NE(functor_handle, nullptr);
  {
    // successful service call
    const std::string SERVICE_REQUEST_VALUE = "service_request_value";
    sup::dto::AnyValue input{ sup::dto::StringType, SERVICE_REQUEST_VALUE };
    sup::dto::AnyValue output;
    EXPECT_EQ(client_stack->Service(input, output), sup::protocol::Success);
  }
  {
    // service call with empty payload fails
    sup::dto::AnyValue input;
    sup::dto::AnyValue output;
    EXPECT_EQ(client_stack->Service(input, output), sup::protocol::ClientTransportEncodingError);
  }
}

TEST_F(ProtocolFactoryUtilsTest, ParseEncoding)
{
  {
    // Default encoding
    sup::dto::AnyValue config = sup::dto::EmptyStruct("MyConfig");
    EXPECT_EQ(ParsePayloadEncoding(config), PayloadEncoding::kBase64);
  }
  {
    // Base64 encoding
    sup::dto::AnyValue config = {{
      { kEncoding, kEncoding_Base64 }
    }, "MyConfig"};
    EXPECT_EQ(ParsePayloadEncoding(config), PayloadEncoding::kBase64);
  }
  {
    // No encoding
    sup::dto::AnyValue config = {{
      { kEncoding, kEncoding_None }
    }, "MyConfig"};
    EXPECT_EQ(ParsePayloadEncoding(config), PayloadEncoding::kNone);
  }
  {
    // Extra field
    sup::dto::AnyValue config = {{
      { "flag", { sup::dto::BooleanType, false }},
      { kEncoding, kEncoding_None }
    }, "MyConfig"};
    EXPECT_EQ(ParsePayloadEncoding(config), PayloadEncoding::kNone);
  }
  {
    // Wrong encoding type
    sup::dto::AnyValue config = {{
      { kEncoding, { sup::dto::BooleanType, false }}
    }, "MyConfig"};
    EXPECT_THROW(ParsePayloadEncoding(config), InvalidOperationException);
  }
  {
    // Unknown encoding
    sup::dto::AnyValue config = {{
      { kEncoding, "UnknownEncoding" }
    }, "MyConfig"};
    EXPECT_THROW(ParsePayloadEncoding(config), InvalidOperationException);
  }
}

TEST_F(ProtocolFactoryUtilsTest, ValidateConfig)
{
  // Expected config field present and of correct type
  sup::dto::AnyValue config = {{
    { "some_name", { sup::dto::UnsignedInteger64Type, 42 } },
    { "flag", false }
  }, "MyConfig"};
  EXPECT_NO_THROW(ValidateConfigurationField(config, "some_name",
                                             sup::dto::UnsignedInteger64Type));
  EXPECT_NO_THROW(ValidateConfigurationField(config, "flag", sup::dto::BooleanType));
  // Expected field missing
  EXPECT_THROW(ValidateConfigurationField(config, "not_present", sup::dto::StringType),
               InvalidOperationException);
  // Expected field with wrong type
  EXPECT_THROW(ValidateConfigurationField(config, "some_name", sup::dto::StringType),
               InvalidOperationException);
  EXPECT_THROW(ValidateConfigurationField(config, "flag", sup::dto::StringType),
               InvalidOperationException);
}