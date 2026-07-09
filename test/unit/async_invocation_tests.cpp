/******************************************************************************
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Marcelo Mariano (External)
 *
 * Copyright (c) : 2010-2026 ITER Organization,
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

#include "test_functor.h"

#include <sup/protocol/async_invocation.h>

#include <sup/protocol/protocol_rpc.h>

#include <sup/dto/anyvalue_helper.h>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace sup::protocol;

namespace
{
const sup::dto::AnyValue kInput{sup::dto::UnsignedInteger32Type, 42u};
const sup::dto::AnyValue kReplyPayload{sup::dto::Float64Type, 3.14};
const sup::dto::uint64 kRequestId{42u};
}  // unnamed namespace

class AsyncInvocationTest : public ::testing::Test
{
};

// Start reports a transport exception when the network client throws.
TEST_F(AsyncInvocationTest, StartTransportException)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue&) -> sup::dto::AnyValue {
    throw std::runtime_error("network down");
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  EXPECT_EQ(invocation.Start(kInput), ClientTransportException);
  EXPECT_FALSE(invocation.IsSynchronous());
}

// A synchronous reply that is not a well-formed RPC reply is rejected at Start().
TEST_F(AsyncInvocationTest, MalformedSynchronousReplyRejected)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue&) { return sup::dto::AnyValue{}; });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  EXPECT_EQ(invocation.Start(kInput), ClientTransportDecodingError);
  EXPECT_FALSE(invocation.IsSynchronous());
}

// PollOnce reports a transport exception when the network client throws during polling.
TEST_F(AsyncInvocationTest, PollOnceTransportException)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    if (utils::GetAsyncInfo(input).second == AsyncCommand::kInitialRequest)
    {
      return utils::CreateAsyncRPCNewRequestReply(kRequestId, PayloadEncoding::kNone);
    }
    throw std::runtime_error("network down");
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  ASSERT_FALSE(invocation.IsSynchronous());
  auto poll = invocation.PollOnce();
  EXPECT_EQ(poll.first, ClientTransportException);
  EXPECT_FALSE(poll.second);
}

// A synchronous error reply is detected as synchronous and propagates the error result.
TEST_F(AsyncInvocationTest, SynchronousErrorReply)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo(
    [](const sup::dto::AnyValue&) { return utils::CreateRPCReply(ServerProtocolException); });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  EXPECT_TRUE(invocation.IsSynchronous());
  auto poll = invocation.PollOnce();
  EXPECT_EQ(poll.first, Success);
  EXPECT_TRUE(poll.second);
  sup::dto::AnyValue output;
  EXPECT_EQ(invocation.GetReply(output), ServerProtocolException);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

// A synchronous success reply is decoded directly, without polling the server.
TEST_F(AsyncInvocationTest, SynchronousSuccessReply)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue&) {
    return utils::CreateRPCReply(Success, kReplyPayload, PayloadEncoding::kBase64);
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  EXPECT_TRUE(invocation.IsSynchronous());
  auto poll = invocation.PollOnce();
  EXPECT_EQ(poll.first, Success);
  EXPECT_TRUE(poll.second);
  sup::dto::AnyValue output;
  EXPECT_EQ(invocation.GetReply(output), Success);
  EXPECT_EQ(output, kReplyPayload);
}

// Full asynchronous flow that succeeds with an empty reply payload.
TEST_F(AsyncInvocationTest, AsyncSuccessWithoutPayload)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    switch (utils::GetAsyncInfo(input).second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(kRequestId, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, {}, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    default:
      return sup::dto::AnyValue{};
    }
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  EXPECT_FALSE(invocation.IsSynchronous());
  auto poll = invocation.PollOnce();
  EXPECT_EQ(poll.first, Success);
  EXPECT_TRUE(poll.second);
  sup::dto::AnyValue output;
  EXPECT_EQ(invocation.GetReply(output), Success);
  EXPECT_TRUE(sup::dto::IsEmptyValue(output));
}

// Full asynchronous flow that succeeds and returns a reply payload.
TEST_F(AsyncInvocationTest, AsyncSuccessWithPayload)
{
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    switch (utils::GetAsyncInfo(input).second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(kRequestId, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      return utils::CreateAsyncRPCPollReply(true, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    default:
      return sup::dto::AnyValue{};
    }
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  EXPECT_FALSE(invocation.IsSynchronous());
  auto poll = invocation.PollOnce();
  EXPECT_EQ(poll.first, Success);
  EXPECT_TRUE(poll.second);
  sup::dto::AnyValue output;
  EXPECT_EQ(invocation.GetReply(output), Success);
  EXPECT_EQ(output, kReplyPayload);
}

// PollOnce reports not-ready until the server signals readiness, then GetReply succeeds.
TEST_F(AsyncInvocationTest, PollNotReadyThenReady)
{
  int poll_count = 0;
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([&poll_count](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    switch (utils::GetAsyncInfo(input).second)
    {
    case AsyncCommand::kInitialRequest:
      return utils::CreateAsyncRPCNewRequestReply(kRequestId, PayloadEncoding::kNone);
    case AsyncCommand::kPoll:
      ++poll_count;
      return utils::CreateAsyncRPCPollReply(poll_count >= 2, PayloadEncoding::kBase64);
    case AsyncCommand::kGetReply:
      return utils::CreateAsyncRPCReply(Success, kReplyPayload, PayloadEncoding::kNone,
                                        AsyncCommand::kGetReply);
    default:
      return sup::dto::AnyValue{};
    }
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  auto first_poll = invocation.PollOnce();
  EXPECT_EQ(first_poll.first, Success);
  EXPECT_FALSE(first_poll.second);
  auto second_poll = invocation.PollOnce();
  EXPECT_EQ(second_poll.first, Success);
  EXPECT_TRUE(second_poll.second);
  sup::dto::AnyValue output;
  EXPECT_EQ(invocation.GetReply(output), Success);
  EXPECT_EQ(output, kReplyPayload);
}

// Invalidate notifies the server with an invalidate command for a pending asynchronous request.
TEST_F(AsyncInvocationTest, InvalidateSendsInvalidateCommand)
{
  bool invalidate_sent = false;
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([&invalidate_sent](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    const auto command = utils::GetAsyncInfo(input).second;
    if (command == AsyncCommand::kInitialRequest)
    {
      return utils::CreateAsyncRPCNewRequestReply(kRequestId, PayloadEncoding::kNone);
    }
    if (command == AsyncCommand::kInvalidate)
    {
      invalidate_sent = true;
      return utils::CreateAsyncRPCReply(Success, {}, PayloadEncoding::kNone,
                                        AsyncCommand::kInvalidate);
    }
    return sup::dto::AnyValue{};
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  invocation.Invalidate();
  EXPECT_TRUE(invalidate_sent);
}

// Invalidate is a no-op for a synchronous reply (no pending request to cancel on the server).
TEST_F(AsyncInvocationTest, InvalidateNoOpWhenSynchronous)
{
  int invalidate_calls = 0;
  ::testing::NiceMock<test::MockFunctor> functor;
  functor.DelegateTo([&invalidate_calls](const sup::dto::AnyValue& input) -> sup::dto::AnyValue {
    if (utils::GetAsyncInfo(input).second == AsyncCommand::kInvalidate)
    {
      ++invalidate_calls;
    }
    return utils::CreateRPCReply(Success, kReplyPayload, PayloadEncoding::kBase64);
  });
  AsyncInvocation invocation{functor, PayloadEncoding::kBase64};
  ASSERT_EQ(invocation.Start(kInput), Success);
  ASSERT_TRUE(invocation.IsSynchronous());
  invocation.Invalidate();
  EXPECT_EQ(invalidate_calls, 0);
}
