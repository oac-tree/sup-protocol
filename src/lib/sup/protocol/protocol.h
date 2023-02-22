/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP RPC protocol stack
 *
 * Description   : The definition and implementation for the RPC protocol stack in SUP
 *
 * Author        : Kevin Meyer
 *
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_PROTOCOL_PROTOCOL_H_
#define SUP_PROTOCOL_PROTOCOL_H_

#include <sup/protocol/protocol_result.h>

namespace sup
{
namespace dto
{
class AnyValue;
}  // namespace dto

namespace protocol
{
/**
 * @brief The Protocol interface is used to provide an abstract re-useable RPC protocol.
 *
 * @details A remote procedure call (RPC) framework provides the ability for a client to invoke
 * functions on a remote server. The rpc unit provides support for an RPC framework that is based on
 * this Protocol class, the universal Invoke method and the ProtocolResult return value. This
 * interface sits between an application layer and a transport layer.
 *
 * Transport layer:
 * A transport layer can define how input, output and result are encoded for transport over the
 * network (typically as payload into well defined request/reply structures). See e.g.
 * ProtocolRPCClient/Server in this software module for a standardized implementation of such a
 * transport layer.
 *
 * Application layer:
 * The details of how the remote function and its parameters are encoded in the AnyValue input
 * and the function ouput is encoded into the AnyValue output are left to a specific implementation.
 */
class Protocol
{
public:
  virtual ~Protocol();

  /**
   * @brief Initiate an RPC call.
   *
   * @param input An input AnyValue structure. This AnyValue cannot be empty.
   * @param output An output AnyValue structure, which may be empty.
   * @return ProtocolResult Success if the call was successfully made, else an error code.
   */
  virtual ProtocolResult Invoke(const sup::dto::AnyValue& input,
                                sup::dto::AnyValue& output) = 0;

  /**
   * @brief A service RPC call. This call will not be forwarded to the encapsulated application
   * interface and serves to provide generic application protocol information to the caller.
   *
   * @param input An input AnyValue structure. This AnyValue cannot be empty.
   * @param output An output AnyValue structure.
   * @return ProtocolResult Success if the call was successfully made, else an error code.
   */
  virtual ProtocolResult Service(const sup::dto::AnyValue& input,
                                 sup::dto::AnyValue& output) = 0;
};

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_H_
