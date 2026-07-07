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

#ifndef SUP_PROTOCOL_ASYNC_INVOCATION_H_
#define SUP_PROTOCOL_ASYNC_INVOCATION_H_

#include <sup/dto/any_functor.h>
#include <sup/dto/anyvalue.h>
#include <sup/dto/basic_scalar_types.h>
#include <sup/protocol/protocol_result.h>
#include <sup/protocol/protocol_rpc.h>

#include <utility>

namespace sup
{
namespace protocol
{
/**
 * @brief Drives a single asynchronous RPC call against an AnyFunctor as discrete, non-blocking
 * steps.
 *
 * @details This class exposes the client side of the asynchronous RPC wire protocol
 * (see protocol_rpc.h) as three explicit steps: Start (send the request), PollOnce (a single,
 * non-blocking readiness check) and GetReply (retrieve the payload once ready). It contains the
 * exact logic that was previously embedded in the blocking loop of ProtocolRPCClient, allowing
 * callers to drive the polling loop themselves (e.g. across ticks of a behavior tree instruction)
 * without spawning threads.
 *
 * @note This class is single threaded: a single instance must not be used concurrently.
 */
class AsyncInvocation
{
public:
  /**
   * @brief Constructor.
   * @param functor Network client used to send the RPC packets. Not owned; must outlive this
   * object.
   * @param encoding Payload encoding to use for the RPC packets.
   */
  AsyncInvocation(sup::dto::AnyFunctor& functor, PayloadEncoding encoding);

  ~AsyncInvocation();

  AsyncInvocation(const AsyncInvocation&) = delete;
  AsyncInvocation(AsyncInvocation&&) = delete;
  AsyncInvocation& operator=(const AsyncInvocation&) = delete;
  AsyncInvocation& operator=(AsyncInvocation&&) = delete;

  /**
   * @brief Send the initial asynchronous request.
   * @param input Application level input payload (cannot be empty).
   * @return Success if the request was accepted (asynchronous handle obtained) or if the server
   * replied synchronously; an error code otherwise.
   *
   * @note When the server chooses to reply synchronously, IsSynchronous() returns true and the
   * reply can be retrieved directly with GetReply() (PollOnce() will report ready immediately).
   */
  ProtocolResult Start(const sup::dto::AnyValue& input);

  /**
   * @brief Perform a single, non-blocking poll to check if the reply is ready.
   * @return A pair {result, ready}. When result is Success, ready indicates whether the reply can
   * be retrieved. When result is not Success, a transport/decoding error occurred.
   */
  std::pair<ProtocolResult, bool> PollOnce();

  /**
   * @brief Retrieve the reply payload. Only meaningful once PollOnce() reported ready.
   * @param output Output payload to be filled on success.
   * @return Success and a filled output, or an error code.
   */
  ProtocolResult GetReply(sup::dto::AnyValue& output);

  /**
   * @brief Best effort notification to the server that this asynchronous request is no longer
   * needed (e.g. when the caller is halted). Safe to call at any time.
   */
  void Invalidate();

  /**
   * @brief Whether the server replied synchronously to Start().
   */
  bool IsSynchronous() const;

private:
  sup::dto::AnyFunctor& m_functor;
  PayloadEncoding m_encoding;
  sup::dto::uint64 m_id;
  bool m_synchronous;
  sup::dto::AnyValue m_sync_reply;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ASYNC_INVOCATION_H_
