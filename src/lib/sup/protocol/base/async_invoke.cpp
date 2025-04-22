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

#include "async_invoke.h"

#include "timing_utils.h"

#include <chrono>
#include <cmath>
#include <future>
#include <memory>

namespace sup
{
namespace protocol
{

class AsyncInvoke::AsyncInvokeImpl
{
public:
  AsyncInvokeImpl(Protocol& protocol, const sup::dto::AnyValue& input, double expiration_sec);
  ~AsyncInvokeImpl();

  bool WaitForReady(double seconds);

  bool IsReadyForRemoval() const;

  AsyncInvoke::Reply GetReply();

  void Invalidate();
private:
  void UpdateLastAccess();
  bool IsExpired() const;
  std::future<AsyncInvoke::Reply> m_future;
  bool m_invalidated;
  sup::dto::uint64 m_last_access;
  sup::dto::uint64 m_expiration_time_ns;
};

AsyncInvoke::AsyncInvoke(Protocol& protocol, const sup::dto::AnyValue& input, double expiration_sec)
  : m_impl{std::make_unique<AsyncInvokeImpl>(protocol, input, expiration_sec)}
{}

AsyncInvoke::~AsyncInvoke() = default;

bool AsyncInvoke::IsReady()
{
  return m_impl->WaitForReady(0.0);
}

bool AsyncInvoke::WaitForReady(double seconds)
{
  return m_impl->WaitForReady(seconds);
}

bool AsyncInvoke::IsReadyForRemoval() const
{
  return m_impl->IsReadyForRemoval();
}

AsyncInvoke::Reply AsyncInvoke::GetReply()
{
  return m_impl->GetReply();
}

void AsyncInvoke::Invalidate()
{
  m_impl->Invalidate();
}

AsyncInvoke::AsyncInvokeImpl::AsyncInvokeImpl(Protocol& protocol,
                                              const sup::dto::AnyValue& input,
                                              double expiration_sec)
  : m_future{}
  , m_invalidated{false}
  , m_last_access{utils::GetCurrentTimestamp()}
  , m_expiration_time_ns{utils::ToNanoseconds(expiration_sec)}
{
  // input is captured with copy, since it may be a temporary object
  auto func = [&protocol, input]() {
    sup::dto::AnyValue output{};
    auto result = protocol.Invoke(input, output);
    return AsyncInvoke::Reply{ result, output };
  };
  m_future = std::async(std::launch::async, func);
}

AsyncInvoke::AsyncInvokeImpl::~AsyncInvokeImpl() = default;

bool AsyncInvoke::AsyncInvokeImpl::WaitForReady(double seconds)
{
  if (!m_future.valid() || m_invalidated)
  {
    return false;
  }
  UpdateLastAccess();
  auto duration = std::chrono::duration<double>(seconds);
  return m_future.wait_for(duration) == std::future_status::ready;
}

bool AsyncInvoke::AsyncInvokeImpl::IsReadyForRemoval() const
{
  if (!m_future.valid())
  {
    return true;
  }
  const bool is_ready = m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  const bool expired = IsExpired();
  const bool is_no_longer_needed = m_invalidated || expired;
  return is_ready && is_no_longer_needed;
}

AsyncInvoke::Reply AsyncInvoke::AsyncInvokeImpl::GetReply()
{
  const AsyncInvoke::Reply failure{ InvalidAsynchronousOperationError, {} };
  if (!m_future.valid() ||
      (m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) ||
      m_invalidated)
  {
    return failure;
  }
  AsyncInvoke::Reply reply{ Success, {} };
  try
  {
    reply = m_future.get();
  }
  catch(...)
  {
    reply = { ServerProtocolException, {} };
  }
  return reply;
}

void AsyncInvoke::AsyncInvokeImpl::Invalidate()
{
  m_invalidated = true;
}

void AsyncInvoke::AsyncInvokeImpl::UpdateLastAccess()
{
  m_last_access = utils::GetCurrentTimestamp();
}

bool AsyncInvoke::AsyncInvokeImpl::IsExpired() const
{
  const auto now = utils::GetCurrentTimestamp();
  return (now - m_last_access) > m_expiration_time_ns;
}

}  // namespace protocol

}  // namespace sup
