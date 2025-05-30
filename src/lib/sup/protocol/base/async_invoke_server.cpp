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

#include "async_invoke_server.h"

namespace sup
{
namespace protocol
{

AsyncInvokeServer::AsyncInvokeServer(Protocol& protocol, double expiration_sec)
  : m_protocol{protocol}
  , m_expiration_sec{expiration_sec}
  , m_invokes{}
  , m_mtx{}
  , m_last_id{0}
{}

AsyncInvokeServer::~AsyncInvokeServer() = default;

sup::dto::AnyValue AsyncInvokeServer::HandleInvoke(const sup::dto::AnyValue& payload,
                                                   PayloadEncoding encoding,
                                                   AsyncCommand command)
{
  if (command == AsyncCommand::kInitialRequest)
  {
    return NewRequest(payload, encoding);
  }
  auto id_info = ExtractAsyncRequestId(payload);
  if (!id_info.first)
  {
    return utils::CreateAsyncRPCReply(ServerTransportDecodingError, command);
  }
  switch (command)
  {
  case AsyncCommand::kPoll:
    return Poll(id_info.second, encoding);
  case AsyncCommand::kGetReply:
    return GetReply(id_info.second, encoding);
  case AsyncCommand::kInvalidate:
    return Invalidate(id_info.second);
  default:
    break;
  }
  return utils::CreateAsyncRPCReply(InvalidAsynchronousOperationError, command);
}

bool AsyncInvokeServer::WaitForReady(sup::dto::uint64 id, double seconds)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto iter = m_invokes.find(id);
  if (iter == m_invokes.end())
  {
    return false;
  }
  return iter->second.WaitForReady(seconds);
}

void AsyncInvokeServer::CleanUpExpiredRequests()
{
  std::lock_guard<std::mutex> lk{m_mtx};
  for (auto iter = m_invokes.begin(); iter != m_invokes.end();)
  {
    if (iter->second.IsReadyForRemoval())
    {
      iter = m_invokes.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

sup::dto::AnyValue AsyncInvokeServer::NewRequest(const sup::dto::AnyValue& payload,
                                                 PayloadEncoding encoding)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto id = GetRequestId();
  (void)m_invokes.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                          std::forward_as_tuple(m_protocol, payload, m_expiration_sec));
  return utils::CreateAsyncRPCNewRequestReply(id, encoding);
}

sup::dto::AnyValue AsyncInvokeServer::Poll(sup::dto::uint64 id, PayloadEncoding encoding)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto iter = m_invokes.find(id);
  if (iter == m_invokes.end())
  {
    return utils::CreateAsyncRPCReply(InvalidRequestIdentifierError, AsyncCommand::kPoll);
  }
  // This indicates an invalid operation. Either the client itself or someone else invalidated
  // the request!
  if (iter->second.IsReadyForRemoval())
  {
    return utils::CreateAsyncRPCReply(InvalidAsynchronousOperationError, AsyncCommand::kPoll);
  }
  return utils::CreateAsyncRPCPollReply(iter->second.IsReady(), encoding);
}

sup::dto::AnyValue AsyncInvokeServer::GetReply(sup::dto::uint64 id, PayloadEncoding encoding)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto iter = m_invokes.find(id);
  if (iter == m_invokes.end())
  {
    return utils::CreateAsyncRPCReply(InvalidRequestIdentifierError, AsyncCommand::kGetReply);
  }
  auto reply = iter->second.GetReply();
  if (iter->second.IsReadyForRemoval())
  {
    (void)m_invokes.erase(iter);
  }
  return utils::CreateAsyncRPCReply(reply.first, reply.second, encoding, AsyncCommand::kGetReply);
}

sup::dto::AnyValue AsyncInvokeServer::Invalidate(sup::dto::uint64 id)
{
  std::lock_guard<std::mutex> lk{m_mtx};
  auto iter = m_invokes.find(id);
  if (iter == m_invokes.end())
  {
    return utils::CreateAsyncRPCReply(InvalidRequestIdentifierError, AsyncCommand::kInvalidate);
  }
  iter->second.Invalidate();
  if (iter->second.IsReadyForRemoval())
  {
    (void)m_invokes.erase(iter);
  }
  return utils::CreateAsyncRPCReply(Success, AsyncCommand::kInvalidate);
}

sup::dto::uint64 AsyncInvokeServer::GetRequestId()
{
  return ++m_last_id;
}

std::pair<bool, sup::dto::uint64> ExtractAsyncRequestId(const sup::dto::AnyValue& payload)
{
  std::pair<bool, sup::dto::uint64> failure{ false, 0 };
  if (!payload.HasField(constants::ASYNC_ID_FIELD_NAME))
  {
    return failure;
  }
  auto& id_field = payload[constants::ASYNC_ID_FIELD_NAME];
  if (id_field.GetType() != sup::dto::UnsignedInteger64Type)
  {
    return failure;
  }
  return { true, id_field.As<sup::dto::uint64>() };
}

}  // namespace protocol

}  // namespace sup
