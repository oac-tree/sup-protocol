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

#ifndef SUP_PROTOCOL_ASYNC_INVOKE_SERVER_H_
#define SUP_PROTOCOL_ASYNC_INVOKE_SERVER_H_

#include "async_invoke.h"

#include <map>
#include <mutex>

namespace sup
{
namespace protocol
{
/**
 * @brief This class manages multiple asynchronous calls of Protocol::Invoke for a specific protocol
 * object.
 */
class AsyncInvokeServer
{
public:
  /**
   * @brief Constructor.
   *
   * @param protocol Protocol to invoke.
   */
  explicit AsyncInvokeServer(Protocol& protocol);
  ~AsyncInvokeServer();

  // No copy/move ctor/assignment:
  AsyncInvokeServer(const AsyncInvokeServer&) = delete;
  AsyncInvokeServer(AsyncInvokeServer&&) = delete;
  AsyncInvokeServer& operator=(const AsyncInvokeServer&) = delete;
  AsyncInvokeServer& operator=(AsyncInvokeServer&&) = delete;

  /**
   * @brief Handle the Protocol::Invoke for an asynchronous request. The payload is already
   * assumed to be decoded.
   *
   * @param payload (possibly decoded) payload of the request.
   * @param encoding Encoding to use for the reply.
   * @param command Asynchronous command.
   * @return Reply to be send back to the client.
   */
  sup::dto::AnyValue HandleInvoke(const sup::dto::AnyValue& payload, PayloadEncoding encoding,
                                  AsyncCommand command);

  /**
   * @brief Wait for a reply to become ready (mainly used to facilitate testing).
   *
   * @param id Identifier of the request.
   * @param seconds Timeout to wait.
   * @return true If the reply was ready within the timeout.
   */
  bool WaitForReady(sup::dto::uint64 id, double seconds);

private:
  sup::dto::AnyValue NewRequest(const sup::dto::AnyValue& payload, PayloadEncoding encoding);
  sup::dto::AnyValue Poll(sup::dto::uint64 id, PayloadEncoding encoding);
  sup::dto::AnyValue GetReply(sup::dto::uint64 id, PayloadEncoding encoding);
  sup::dto::AnyValue Invalidate(sup::dto::uint64 id);
  sup::dto::uint64 GetRequestId();

  Protocol& m_protocol;
  std::map<sup::dto::uint64, AsyncInvoke> m_invokes;
  std::mutex m_mtx;
  sup::dto::uint64 m_last_id;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ASYNC_INVOKE_SERVER_H_
