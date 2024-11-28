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

#include <sup/protocol/base/async_invoke_server.h>

#include <gtest/gtest.h>

#include <future>

using namespace sup::protocol;

class AsyncRequestServerTest : public ::testing::Test
{
protected:
  AsyncRequestServerTest();
  virtual ~AsyncRequestServerTest();
};

ProtocolResult ExtractProtocolResult(const sup::dto::AnyValue& reply)
{
  return ProtocolResult{reply[constants::REPLY_RESULT].As<sup::dto::uint32>()};
}

AsyncCommand ExtractAsyncCommand(const sup::dto::AnyValue& reply)
{
  auto command_id = reply[constants::ASYNC_COMMAND_FIELD_NAME].As<sup::dto::uint32>();
  return static_cast<AsyncCommand>(command_id);
}

TEST_F(AsyncRequestServerTest, NoActiveRequests)
{
  // Check status of AsyncInvokeServer after construction (without any active requests).
  const sup::dto::AnyValue id_payload = {{
    { constants::ASYNC_ID_FIELD_NAME, { sup::dto::UnsignedInteger64Type, 42u }}
  }};
  std::promise<void> go;
  test::AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvokeServer async_server{protocol};
  {
    // Poll non-existing request
    auto reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
    EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);
    EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  }
  {
    // GetReply for non-existing request
    auto reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone,
                                           AsyncCommand::kGetReply);
    EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);
    EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  }
  {
    // Invalidate non-existing request
    auto reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone,
                                           AsyncCommand::kInvalidate);
    EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);
    EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kInvalidate);
  }
}

TEST_F(AsyncRequestServerTest, SimpleScalarRequest)
{
  test::TestProtocol protocol{};
  AsyncInvokeServer async_server{protocol};
  const sup::dto::AnyValue payload{ sup::dto::UnsignedInteger8Type, 1 };
  auto reply = async_server.HandleInvoke(payload, PayloadEncoding::kNone,
                                         AsyncCommand::kInitialRequest);

}

TEST_F(AsyncRequestServerTest, SingleRequest)
{
  // Check status of AsyncInvokeServer after launching a single request
  const sup::dto::AnyValue input{ sup::dto::StringType, "This is the request payload" };
  std::promise<void> go;
  test::AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvokeServer async_server{protocol};

  // Launch new request (first id should be 1)
  auto reply = async_server.HandleInvoke(input, PayloadEncoding::kNone,
                                         AsyncCommand::kInitialRequest);
  auto id = test::ExtractRequestId(reply);
  EXPECT_EQ(id, 1u);

  // Request should not be ready
  const sup::dto::AnyValue id_payload = {{
    { constants::ASYNC_ID_FIELD_NAME, id }
  }};
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  EXPECT_EQ(ExtractProtocolResult(reply), Success);
  auto is_ready = test::ExtractReadyStatus(reply);
  EXPECT_FALSE(is_ready);

  // GetReply returns an error ProtocolResult since the reply is not ready
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidAsynchronousOperationError);

  // Make result ready
  go.set_value();
  EXPECT_TRUE(async_server.WaitForReady(id, 1.0));

  // Poll indicates readiness
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  EXPECT_EQ(ExtractProtocolResult(reply), Success);
  is_ready = test::ExtractReadyStatus(reply);
  EXPECT_TRUE(is_ready);

  // GetReply returns the expected reply
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractProtocolResult(reply), Success);
  EXPECT_EQ(reply[constants::REPLY_PAYLOAD], input);

  // Poll no longer knows about that identifier
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);

  // Same for GetReply
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);
}

TEST_F(AsyncRequestServerTest, Invalidate)
{
  // Check status of AsyncInvokeServer after launching a single request
  const sup::dto::AnyValue input{ sup::dto::StringType, "This is the request payload" };
  std::promise<void> go;
  test::AsyncRequestTestProtocol protocol{go.get_future()};
  AsyncInvokeServer async_server{protocol};

  // Launch new request (first id should be 1)
  auto reply = async_server.HandleInvoke(input, PayloadEncoding::kNone,
                                         AsyncCommand::kInitialRequest);
  auto id = test::ExtractRequestId(reply);
  EXPECT_EQ(id, 1u);

  // Request should not be ready
  const sup::dto::AnyValue id_payload = {{
    { constants::ASYNC_ID_FIELD_NAME, id }
  }};
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  EXPECT_EQ(ExtractProtocolResult(reply), Success);
  auto is_ready = test::ExtractReadyStatus(reply);
  EXPECT_EQ(is_ready, 0);

  // GetReply returns an error ProtocolResult since the reply is not ready
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidAsynchronousOperationError);

  // Make result ready and then invalidate
  go.set_value();
  EXPECT_TRUE(async_server.WaitForReady(id, 1.0));

  // Invalidate the request
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kInvalidate);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kInvalidate);
  EXPECT_EQ(ExtractProtocolResult(reply), Success);

  // Poll no longer knows about that identifier
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kPoll);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kPoll);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);

  // Same for GetReply
  reply = async_server.HandleInvoke(id_payload, PayloadEncoding::kNone, AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractAsyncCommand(reply), AsyncCommand::kGetReply);
  EXPECT_EQ(ExtractProtocolResult(reply), InvalidRequestIdentifierError);
}

AsyncRequestServerTest::AsyncRequestServerTest() = default;

AsyncRequestServerTest::~AsyncRequestServerTest() = default;
