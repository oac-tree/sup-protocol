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
 * SPDX-License-Identifier: MIT
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file LICENSE located in the top level directory
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
   * @param expiration_sec Time in seconds for an asynchronous invoke to become expired.
   */
  explicit AsyncInvokeServer(Protocol& protocol, double expiration_sec);
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

  /**
   * @brief Look for expired requests and clean up the resources associated with them.
   */
  void CleanUpExpiredRequests();

private:
  sup::dto::AnyValue NewRequest(const sup::dto::AnyValue& payload, PayloadEncoding encoding);
  sup::dto::AnyValue Poll(sup::dto::uint64 id, PayloadEncoding encoding);
  sup::dto::AnyValue GetReply(sup::dto::uint64 id, PayloadEncoding encoding);
  sup::dto::AnyValue Invalidate(sup::dto::uint64 id);
  sup::dto::uint64 GetRequestId();

  Protocol& m_protocol;
  const double m_expiration_sec;
  std::map<sup::dto::uint64, AsyncInvoke> m_invokes;
  std::mutex m_mtx;
  sup::dto::uint64 m_last_id;
};

/**
 * @brief Try to extract the unique id of the asynchronous request from the payload.
 *
 * @param payload Payload to use for unique id extraction.
 * @return The first member of the pair indicates if the extraction was successful, and if so,
 * the second member will contain the extracted unique id.
 */
std::pair<bool, sup::dto::uint64> ExtractAsyncRequestId(const sup::dto::AnyValue& payload);

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ASYNC_INVOKE_SERVER_H_
