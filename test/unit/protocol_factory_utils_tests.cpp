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

#include <sup/protocol/protocol_factory_utils.h>
#include <sup/protocol/protocol_rpc.h>

#include <sup/dto/any_functor.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

class TestServerFunctor : public RPCServerInterface
{
public:
  TestServerFunctor(sup::dto::AnyFunctor& protocol_server, TestServerFunctor*& handle)
    : m_protocol_server{protocol_server}
  {
    handle = this;
  }
  ~TestServerFunctor() = default;

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

  std::unique_ptr<RPCServerInterface> CreateTestServerFunctor(
    sup::dto::AnyFunctor& protocol_server, TestServerFunctor*& handle) {
      return std::unique_ptr<RPCServerInterface>(new TestServerFunctor(protocol_server, handle));
    }
};

TEST_F(ProtocolFactoryUtilsTest, CreateRPCServerStack)
{
  test::TestProtocol test_protocol{};
  TestServerFunctor* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestServerFunctor(protocol_server, server_handle);
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
  TestServerFunctor* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestServerFunctor(protocol_server, server_handle);
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
  TestServerFunctor* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestServerFunctor(protocol_server, server_handle);
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
  TestServerFunctor* server_handle = nullptr;
  auto factory_func = [this, &server_handle](sup::dto::AnyFunctor& protocol_server) {
    return CreateTestServerFunctor(protocol_server, server_handle);
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

