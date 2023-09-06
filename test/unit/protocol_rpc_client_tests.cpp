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

#include <sup/protocol/protocol_rpc_client.h>
#include <sup/protocol/protocol_rpc_server.h>

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/exceptions.h>
#include <sup/dto/any_functor.h>
#include <sup/dto/anyvalue.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

static const std::string BAD_REPLY_FIELD = "bad_reply";
static const std::string THROW_FIELD = "throw";

class TestFunctor : public sup::dto::AnyFunctor
{
public:
  TestFunctor();
  ~TestFunctor();

  sup::dto::AnyValue operator()(const sup::dto::AnyValue& input) override;

  sup::dto::AnyValue GetLastRequest() const;

private:
  std::unique_ptr<sup::dto::AnyValue> m_last_request;
};

class ProtocolRPCClientTest : public ::testing::Test
{
protected:
  ProtocolRPCClientTest();
  virtual ~ProtocolRPCClientTest();

  sup::dto::AnyFunctor& GetTestFunctor();
  sup::dto::AnyFunctor& GetServerFunctor();

  TestFunctor m_test_functor;
  test::TestProtocol m_test_protocol;
  ProtocolRPCServer m_protocol_server;
};

TEST_F(ProtocolRPCClientTest, Construction)
{
  EXPECT_NO_THROW(ProtocolRPCClient client{GetTestFunctor()});
}

TEST_F(ProtocolRPCClientTest, InvokeEmptyInput)
{
  ProtocolRPCClient client{GetTestFunctor()};
  sup::dto::AnyValue output;
  EXPECT_EQ(client.Invoke(sup::dto::AnyValue{}, output), ClientTransportEncodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
  EXPECT_TRUE(sup::dto::IsEmptyValue(m_test_functor.GetLastRequest()));
}

TEST_F(ProtocolRPCClientTest, InvokeScalarInput)
{
  ProtocolRPCClient client{GetTestFunctor()};
  sup::dto::AnyValue input{sup::dto::SignedInteger32Type, 42};
  sup::dto::AnyValue output;
  EXPECT_EQ(client.Invoke(input, output), Success);
  EXPECT_FALSE(sup::dto::IsEmptyValue(output));
  EXPECT_EQ(input, output);
  auto last_request = m_test_functor.GetLastRequest();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_request));
  EXPECT_TRUE(utils::CheckRequestFormat(last_request));
  EXPECT_EQ(last_request.GetTypeName(), constants::REQUEST_TYPE_NAME);
  EXPECT_TRUE(last_request.HasField(constants::REQUEST_PAYLOAD));
  auto payload = utils::ExtractRPCPayload(last_request, constants::REQUEST_PAYLOAD);
  EXPECT_EQ(payload.GetType(), sup::dto::SignedInteger32Type);
  EXPECT_EQ(payload, input);
}

TEST_F(ProtocolRPCClientTest, InvokeBadReply)
{
  ProtocolRPCClient client{GetTestFunctor()};
  sup::dto::AnyValue input = {{
    {BAD_REPLY_FIELD, {sup::dto::BooleanType, true}}
  }};
  sup::dto::AnyValue output{};
  EXPECT_EQ(client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));

  auto last_request = m_test_functor.GetLastRequest();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_request));
  EXPECT_TRUE(utils::CheckRequestFormat(last_request));
  EXPECT_EQ(last_request.GetTypeName(), constants::REQUEST_TYPE_NAME);
  EXPECT_TRUE(last_request.HasField(constants::REQUEST_PAYLOAD));
  auto payload = utils::ExtractRPCPayload(last_request, constants::REQUEST_PAYLOAD);
  EXPECT_EQ(payload.GetType(), input.GetType());
  EXPECT_EQ(payload, input);
}

TEST_F(ProtocolRPCClientTest, InvokeBadOutput)
{
  ProtocolRPCClient client{GetTestFunctor()};
  sup::dto::AnyValue input{sup::dto::SignedInteger32Type, 42};
  sup::dto::AnyValue output_start{sup::dto::StringType, "start_value"};
  sup::dto::AnyValue output{output_start};
  EXPECT_EQ(client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_FALSE(sup::dto::IsEmptyValue(output));
  EXPECT_EQ(output, output_start);
}

TEST_F(ProtocolRPCClientTest, FunctorThrows)
{
  ProtocolRPCClient client{GetTestFunctor()};
  sup::dto::AnyValue input = {{
    {THROW_FIELD, {sup::dto::BooleanType, true}}
  }};
  sup::dto::AnyValue output{};
  EXPECT_EQ(client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));

  auto last_request = m_test_functor.GetLastRequest();
  EXPECT_FALSE(sup::dto::IsEmptyValue(last_request));
  EXPECT_TRUE(utils::CheckRequestFormat(last_request));
  EXPECT_EQ(last_request.GetTypeName(), constants::REQUEST_TYPE_NAME);
  EXPECT_TRUE(last_request.HasField(constants::REQUEST_PAYLOAD));
  auto payload = utils::ExtractRPCPayload(last_request, constants::REQUEST_PAYLOAD);
  EXPECT_EQ(payload.GetType(), input.GetType());
  EXPECT_EQ(payload, input);
}

TEST_F(ProtocolRPCClientTest, ServiceMethod)
{
  ProtocolRPCClient client{GetTestFunctor()};
  {
    // successful service call
    const std::string SERVICE_REQUEST_VALUE = "service_request_value";
    sup::dto::AnyValue input{ sup::dto::StringType, SERVICE_REQUEST_VALUE };
    sup::dto::AnyValue output;
    EXPECT_EQ(client.Service(input, output), sup::protocol::Success);
  }
  {
    // service call with empty payload fails
    sup::dto::AnyValue input;
    sup::dto::AnyValue output;
    EXPECT_EQ(client.Service(input, output), sup::protocol::ClientTransportEncodingError);
  }
  {
    // underlying functor throws
    sup::dto::AnyValue input = {
      { THROW_FIELD, {sup::dto::BooleanType, true}}
    };
    sup::dto::AnyValue output;
    EXPECT_EQ(client.Service(input, output), sup::protocol::ClientTransportDecodingError);
  }
  {
    // wrong output type provided
    const std::string SERVICE_REQUEST_VALUE = "service_request_value";
    sup::dto::AnyValue input{ sup::dto::StringType, SERVICE_REQUEST_VALUE };
    sup::dto::AnyValue output{ sup::dto::BooleanType };
    EXPECT_EQ(client.Service(input, output), sup::protocol::ClientTransportDecodingError);
  }
}

TEST_F(ProtocolRPCClientTest, ApplicationProtocolInfo)
{
  ProtocolRPCClient client{GetServerFunctor()};
  // Successful retrieval of application protocol information
  m_test_protocol.SetFailForServiceRequest(false);
  auto protocol_info = utils::GetApplicationProtocolInfo(client);
  EXPECT_EQ(protocol_info.m_application_type, test::TEST_PROTOCOL_TYPE);
  EXPECT_EQ(protocol_info.m_application_version, test::TEST_PROTOCOL_VERSION);
  // Failing retrieval of application protocol information
  m_test_protocol.SetFailForServiceRequest(true);
  protocol_info = utils::GetApplicationProtocolInfo(client);
  EXPECT_TRUE(protocol_info.m_application_type.empty());
  EXPECT_TRUE(protocol_info.m_application_version.empty());
}

TestFunctor::TestFunctor()
  : m_last_request{}
{}

TestFunctor::~TestFunctor() = default;

sup::dto::AnyValue TestFunctor::operator()(const sup::dto::AnyValue& input)
{
  m_last_request.reset(new sup::dto::AnyValue(input));
  auto encoding = utils::GetPacketEncoding(input);
  bool normal_request = input.HasField(constants::REQUEST_PAYLOAD);
  auto query = normal_request ? utils::ExtractRPCPayload(input, constants::REQUEST_PAYLOAD)
                              : utils::ExtractRPCPayload(input, constants::SERVICE_REQUEST_PAYLOAD);
  if (query.HasField(BAD_REPLY_FIELD) && query[BAD_REPLY_FIELD].As<bool>())
  {
    return sup::dto::AnyValue{{{"BadReplyFormat", {sup::dto::BooleanType, true}}}};
  }
  if (query.HasField(THROW_FIELD) && query[THROW_FIELD].As<bool>())
  {
    throw std::runtime_error("Throwing on demand");
  }
  return normal_request ? utils::CreateRPCReply(Success, query, encoding)
                        : utils::CreateServiceReply(Success, query, encoding);
}

sup::dto::AnyValue TestFunctor::GetLastRequest() const
{
  if (!m_last_request)
  {
    return {};
  }
  sup::dto::AnyValue result{*m_last_request};
  return result;
}

ProtocolRPCClientTest::ProtocolRPCClientTest()
  : m_test_functor{}
  , m_test_protocol{}
  , m_protocol_server{m_test_protocol}
{}

ProtocolRPCClientTest::~ProtocolRPCClientTest() = default;

sup::dto::AnyFunctor& ProtocolRPCClientTest::GetTestFunctor()
{
  return m_test_functor;
}

sup::dto::AnyFunctor& ProtocolRPCClientTest::GetServerFunctor()
{
  return m_protocol_server;
}
