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

#include <sup/protocol/protocol_rpc_client.h>

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/exceptions.h>

#include <gtest/gtest.h>

using namespace sup::protocol;

using ::testing::_;
using ::testing::AtLeast;

const sup::dto::AnyValue kReplyPayload{ sup::dto::Float64Type, 3.14 };

class ProtocolRPCClientAsyncTest : public ::testing::Test
{
protected:
  ProtocolRPCClientAsyncTest();
  virtual ~ProtocolRPCClientAsyncTest();
};

TEST_F(ProtocolRPCClientAsyncTest, EmptyPayload)
{
  // Createmock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input{};
  sup::dto::AnyValue output{};
  // Check Invoke fails before calling functor:
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportEncodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, SuccessWithoutReplyPayload)
{
  // Create function that imitates successful asynchronous communication:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, {}, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check successful asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), Success);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, SuccessWithReplyPayload)
{
  // Create function that imitates successful asynchronous communication:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check successful asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), Success);
  EXPECT_EQ(output, kReplyPayload);
}

TEST_F(ProtocolRPCClientAsyncTest, UnknownEncodingInNewRequest)
{
  // Create function that uses an unknown encoding in its reply to a new request:
  auto unknown_encoding_reply = utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kBase64);
  unknown_encoding_reply[constants::ENCODING_FIELD_NAME].ConvertFrom(77);
  auto func = [unknown_encoding_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return unknown_encoding_reply;
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(1);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, MissingIdInNewRequest)
{
  // Create function that replies to a new request without providing an id:
  auto missing_id_reply = utils::CreateAsyncRPCReply(Success, AsyncCommand::kInitialRequest);
  auto func = [missing_id_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return missing_id_reply;
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(1);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, ThrowOnNewRequest)
{
  // Create function that throws on receiving a new asynchronous request:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      throw InvalidOperationException("Not supported");
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kBase64,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(1);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportException);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, UnknownEncodingInPoll)
{
  // Create function that uses an unknown encoding in its reply to a poll:
  auto unknown_encoding_reply = utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
  unknown_encoding_reply[constants::ENCODING_FIELD_NAME].ConvertFrom(77);
  auto func = [unknown_encoding_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return unknown_encoding_reply;
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(2);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, MissingReadyStateInPoll)
{
  // Create function that replies to a poll without providing a ready state:
  auto missing_ready_reply = utils::CreateAsyncRPCReply(Success, AsyncCommand::kPoll);
  auto func = [missing_ready_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return missing_ready_reply;
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(2);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, PollTimeout)
{
  // Create function that never replies ready to a poll:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(false, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      throw InvalidOperationException("Reply is never ready");
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.1, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(AtLeast(2));
  EXPECT_EQ(rpc_client.Invoke(input, output), AsynchronousProtocolTimeout);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, ThrowOnPoll)
{
  // Create function that throws on receiving a poll request:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kBase64);
    case AsyncCommand::kPoll:
      throw InvalidOperationException("Not supported");
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(2);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportException);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, UnknownEncodingInGetReply)
{
  // Create function that uses an unknown encoding in its reply to a get reply:
  auto unknown_encoding_reply =
    utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kBase64,
                               AsyncCommand::kGetReply);
  unknown_encoding_reply[constants::ENCODING_FIELD_NAME].ConvertFrom(77);
  auto func = [unknown_encoding_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return unknown_encoding_reply;
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, NoProtocolResultInGetReply)
{
  // Create function that uses a reply without protocol result:
  sup::dto::AnyValue missing_result_reply = {{
    { "invalid", {sup::dto::UnsignedInteger32Type, 42} }
  }, constants::REPLY_TYPE_NAME};
  auto func = [missing_result_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return missing_result_reply;
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, InvalidPayloadInGetReply)
{
  // Create function that uses a reply with invalid encoding of the payload:
  auto invalid_payload_reply =
    utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kBase64,
                               AsyncCommand::kGetReply);
  invalid_payload_reply[constants::REPLY_PAYLOAD] = 42u;  // cannot be decoded!
  auto func = [invalid_payload_reply](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return invalid_payload_reply;
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check for correct error in reponse to Invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportDecodingError);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

TEST_F(ProtocolRPCClientAsyncTest, ThrowOnGetReply)
{
  // Create function that throws on receiving a get reply request:
  auto func = [](const sup::dto::AnyValue& input) {
    auto async_info = utils::GetAsyncInfo(input);
    if (!async_info.first)
    {
      throw InvalidOperationException("No synchronous support");
    }
    switch (async_info.second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(42u, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      throw InvalidOperationException("Not supported");
    }
    throw InvalidOperationException("Unknown async command");
  };
  // Inject function into mock functor:
  ::testing::StrictMock<test::MockFunctor> mock_functor;
  mock_functor.DelegateTo(func);
  // Create client:
  ProtocolRPCClientConfig client_config{PayloadEncoding::kBase64, 0.2, 0.02};
  ProtocolRPCClient rpc_client{mock_functor, client_config};
  sup::dto::AnyValue input { sup::dto::UnsignedInteger32Type, 42u };
  sup::dto::AnyValue output{};
  // Check asynchronous invoke:
  EXPECT_CALL(mock_functor, CallOperator(_)).Times(3);
  EXPECT_EQ(rpc_client.Invoke(input, output), ClientTransportException);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

ProtocolRPCClientAsyncTest::ProtocolRPCClientAsyncTest() = default;

ProtocolRPCClientAsyncTest::~ProtocolRPCClientAsyncTest() = default;
