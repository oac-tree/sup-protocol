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

#ifndef SUP_PROTOCOL_ASYNC_INVOKE_H_
#define SUP_PROTOCOL_ASYNC_INVOKE_H_

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/protocol.h>

#include <memory>
#include <utility>

namespace sup
{
namespace protocol
{
/**
 * @brief This class handles asynchronous calls to Protocol::Invoke.
 *
 * @note Clients of this class are responsible for thread safety. Clients should also be aware that
 * if IsReady() returns false, this does not necessarily mean the encapsulated thread is still
 * running. IsReady returns false also when the reply was already retrieved (after GetReply) or no
 * longer needed (after Invalidate).
 */
class AsyncInvoke
{
public:
  using Reply = std::pair<ProtocolResult, sup::dto::AnyValue>;

  /**
   * @brief Constructor that will immediately launch a thread that calls Protocol::Invoke on the
   * given protocol with the given input.
   *
   * @param protocol Protocol to invoke.
   * @param input AnyValue to pass as input to Protocol::Invoke.
   * @param expiration_sec Time in seconds for an asynchronous invoke to become expired.
   */
  AsyncInvoke(Protocol& protocol, const sup::dto::AnyValue& input, double expiration_sec);
  ~AsyncInvoke();

  // No copy/move ctor/assignment:
  AsyncInvoke(const AsyncInvoke&) = delete;
  AsyncInvoke(AsyncInvoke&&) = delete;
  AsyncInvoke& operator=(const AsyncInvoke&) = delete;
  AsyncInvoke& operator=(AsyncInvoke&&) = delete;

  /**
   * @brief Check if a reply is ready to be retrieved.
   *
   * @return true if a reply is ready to be retrieved.
   */
  bool IsReady();

  /**
   * @brief Check if a reply is ready to be retrieved within the given timeout.
   *
   * @return true if a reply is ready to be retrieved.
   */
  bool WaitForReady(double seconds);

  /**
   * @brief Check if this AsyncInvoke object is ready for destruction, i.e. the encapsulated thread
   * has finished and the reply was already retrieved or no longer needed.
   *
   * @return true if this AsyncInvoke object is ready for destruction.
   */
  bool IsReadyForRemoval() const;

  /**
   * @brief Retrieve the reply from the encapsulated thread. This is only possible if IsReady() would
   * return true.
   *
   * @return The reply from the encapsulated thread or a failure reply if it was not ready.
   */
  Reply GetReply();

  /**
   * @brief Indicate that the reply of the encapsulated thread is no longer needed.
   */
  void Invalidate();
private:
  class AsyncInvokeImpl;
  std::unique_ptr<AsyncInvokeImpl> m_impl;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ASYNC_INVOKE_H_
