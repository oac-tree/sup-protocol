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

#include "async_invoke.h"

#include <chrono>
#include <cmath>
#include <future>

namespace sup
{
namespace protocol
{

class AsyncInvoke::AsyncInvokeImpl
{
public:
  AsyncInvokeImpl(Protocol& protocol, const sup::dto::AnyValue& input);
  ~AsyncInvokeImpl();

  bool WaitForReady(double seconds) const;

  bool IsReadyForRemoval() const;

  AsyncInvoke::Reply GetReply();

  void Invalidate();
private:
  std::future<AsyncInvoke::Reply> m_future;
  bool m_invalidated;
};

AsyncInvoke::AsyncInvoke(Protocol& protocol, const sup::dto::AnyValue& input)
  : m_impl{new AsyncInvokeImpl(protocol, input)}
{}

AsyncInvoke::~AsyncInvoke() = default;

bool AsyncInvoke::IsReady() const
{
  return m_impl->WaitForReady(0.0);
}

bool AsyncInvoke::WaitForReady(double seconds) const
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
                                                 const sup::dto::AnyValue& input)
  : m_future{}
  , m_invalidated{false}
{
  auto func = [&protocol, &input]() -> AsyncInvoke::Reply {
    sup::dto::AnyValue output{};
    auto result = protocol.Invoke(input, output);
    return { result, output };
  };
  m_future = std::async(std::launch::async, func);
}

AsyncInvoke::AsyncInvokeImpl::~AsyncInvokeImpl()
{}

bool AsyncInvoke::AsyncInvokeImpl::WaitForReady(double seconds) const
{
  if (!m_future.valid() || m_invalidated)
  {
    return false;
  }
  auto duration = std::chrono::nanoseconds(std::lround(seconds * 1e9));
  return m_future.wait_for(duration) == std::future_status::ready;
}

bool AsyncInvoke::AsyncInvokeImpl::IsReadyForRemoval() const
{
  if (!m_future.valid())
  {
    return true;
  }
  bool is_ready = m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  return is_ready && m_invalidated;
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
    return { ServerProtocolException, {} };
  }
  return reply;
}

void AsyncInvoke::AsyncInvokeImpl::Invalidate()
{
  m_invalidated = true;
}

}  // namespace protocol

}  // namespace sup
