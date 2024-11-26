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

#include <sup/protocol/async_request.h>

#include <chrono>
#include <future>

namespace sup
{
namespace protocol
{

class AsyncRequest::AsyncRequestImpl
{
public:
  AsyncRequestImpl(Protocol& protocol, const sup::dto::AnyValue& input);
  ~AsyncRequestImpl();

  bool IsReady() const;

  bool IsExpired() const;

  AsyncRequest::Reply GetReply();

  void Invalidate();
private:
  std::future<AsyncRequest::Reply> m_future;
  bool m_invalidated;
};

AsyncRequest::AsyncRequest(Protocol& protocol, const sup::dto::AnyValue& input)
  : m_impl{new AsyncRequestImpl(protocol, input)}
{}

AsyncRequest::~AsyncRequest() = default;

bool AsyncRequest::IsReady() const
{
  return m_impl->IsReady();
}

bool AsyncRequest::IsExpired() const
{
  return m_impl->IsExpired();
}

AsyncRequest::Reply AsyncRequest::GetReply()
{
  return m_impl->GetReply();
}

void AsyncRequest::Invalidate()
{
  m_impl->Invalidate();
}

AsyncRequest::AsyncRequestImpl::AsyncRequestImpl(Protocol& protocol,
                                                 const sup::dto::AnyValue& input)
  : m_future{}
  , m_invalidated{false}
{
  auto func = [&protocol, &input]() -> AsyncRequest::Reply {
    sup::dto::AnyValue output{};
    auto result = protocol.Invoke(input, output);
    return { result, output };
  };
  m_future = std::async(std::launch::async, func);
}

AsyncRequest::AsyncRequestImpl::~AsyncRequestImpl()
{}

bool AsyncRequest::AsyncRequestImpl::IsReady() const
{
  if (!m_future.valid())
  {
    return false;
  }
  return m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool AsyncRequest::AsyncRequestImpl::IsExpired() const
{
  if (!m_future.valid())
  {
    return true;
  }
  bool is_ready = m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
  return is_ready && m_invalidated;
}

AsyncRequest::Reply AsyncRequest::AsyncRequestImpl::GetReply()
{
  const AsyncRequest::Reply failure{ InvalidAsynchronousOperationError, {} };
  if (!m_future.valid() ||
      (m_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) ||
      m_invalidated)
  {
    return failure;
  }
  m_invalidated = true;
  return m_future.get();
}

void AsyncRequest::AsyncRequestImpl::Invalidate()
{
  m_invalidated = true;
}

}  // namespace protocol

}  // namespace sup
