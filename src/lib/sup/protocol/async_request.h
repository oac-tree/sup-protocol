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

#ifndef SUP_PROTOCOL_ASYNC_REQUEST_H_
#define SUP_PROTOCOL_ASYNC_REQUEST_H_

#include <sup/protocol/protocol_rpc.h>
#include <sup/protocol/protocol.h>

#include <memory>
#include <utility>

namespace sup
{
namespace protocol
{

class AsyncRequest
{
public:
  using Reply = std::pair<ProtocolResult, sup::dto::AnyValue>;

  AsyncRequest(Protocol& protocol, const sup::dto::AnyValue& input);
  ~AsyncRequest();

  bool IsReady() const;

  bool IsExpired() const;

  Reply GetReply() const;

  void Invalidate();
private:
  class AsyncRequestImpl;
  std::unique_ptr<AsyncRequestImpl> m_impl;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_ASYNC_REQUEST_H_
