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
 * Copyright (c) : 2010-2025 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ******************************************************************************/

#ifndef SUP_PROTOCOL_PROTOCOL_RESULT_H_
#define SUP_PROTOCOL_PROTOCOL_RESULT_H_

#include <string>

namespace sup
{
namespace protocol
{

/**
 * @brief Start value for the general application errors.
 *
 * @details This is the start value for error codes defined by generic frameworks that link
 * generic interfaces with the Protocol.
 * @sa ProtocolResult
 */
const unsigned int GENERIC_APPLICATION_ERROR_START = 100u;

/**
 * @brief Start value for application-specific errors.
 *
 * @sa ProtocolResult
 */
const unsigned int SPECIFIC_APPLICATION_ERROR_START = 1000u;

/**
 * @brief The ProtocolResult class represents the return value from a Protocol operation.
 *
 * @details The return value describes whether the function call (represented by the Protocol
 * interface) has succeeded or not. If the call was not successful, then the value provides
 * information on probable failure modes.
 *
 * There are three categories of possible values:
 *  - Network and transport layer status: Applies to the lowest levels of the communication stack.
 *  - Generic application errors: Applies to generic frameworks that process the protocol.
 *  - Specific application errors: Applies to application-specific issues with the protocol.
 *
 * The values in each category have fixed ranges with the following start values:
 *  - Network and transport layer status: 0
 *  - Generic application errors: @c GENERIC_APPLICATION_ERROR_START
 *  - Specific application errors: @c SPECIFIC_APPLICATION_ERROR_START
 *
 * The appropriate start value must be used When defining errors for the relevant category. For
 * example, to define values for general application errors:
 *
 * @code{.cpp}
  namespace status
  {
  enum ApplicationErrors
  {
    EMPTY_INPUT =
        GENERIC_APPLICATION_ERROR_START,  ///< Empty input, can not extract function information.
    UNKNOWN_FUNCTION,                     ///< The requested function does not exist.
    BAD_REGISTRATION,                     ///< Problem with the RegisteredFunction parameters.
    ASSIGNMENT_ERROR,                     ///< Unable to assign output value.
  };
  }
 * @endcode
 *
 */
class ProtocolResult
{
public:
  /**
   * @brief Default constructor implies success.
   */
  ProtocolResult();

  ~ProtocolResult();

  /**
   * @brief Construct a new ProtocolResult object for ApplicationSpecific results.
   */
  explicit ProtocolResult(unsigned int value) noexcept;

  /**
   * @brief Copy/move construction and assigment.
   */
  ProtocolResult(const ProtocolResult& other);
  ProtocolResult& operator=(const ProtocolResult& other) &;
  ProtocolResult(ProtocolResult&&) noexcept;
  ProtocolResult& operator=(ProtocolResult&&) & noexcept;

  /**
   * @brief Get status as integer.
   *
   * @return Application Specific error result as integer.
   */
  unsigned int GetValue() const;

  /**
   * @brief Comparison operators.
   */
  bool operator==(const ProtocolResult& other) const;
  bool operator!=(const ProtocolResult& other) const;

private:
  unsigned int m_value;
};

/**
 * @brief Return a string representation of the ProtocolResult.
 *
 * @param result enumeration input.
 * @return std::string output.
 */
std::string ProtocolResultToString(const ProtocolResult& result);

/**
 * @brief Result returned for a successful call.
*/
extern const ProtocolResult Success;
/**
 * @brief Result returned when the network layer cannot properly exchange messages between
 * client and server.
*/
extern const ProtocolResult NotConnected;
/**
 * @brief Error when the network client cannot encode the transport packet into a network packet.
 *
 * Example: the format of the AnyValue, representing a transport packet, is not supported for
 * translation into a pvxs::Value (network packet). This happens when some allowed AnyValue types
 * are not supported in the underlying network implementation, such as arrays of structures.
*/
extern const ProtocolResult ClientNetworkEncodingError;
/**
 * @brief Error when the network server cannot encode the transport packet into a network packet.
 *
 * Example: the format of the AnyValue, representing a transport packet, is not supported for
 * translation into a pvxs::Value (network packet). This happens when some allowed AnyValue types
 * are not supported in the underlying network implementation, such as arrays of structures.
*/
extern const ProtocolResult ServerNetworkEncodingError;
/**
 * @brief Error when the network client cannot decode the network packet into a transport packet.
 *
 * Example: the received pvxs::Value (network packet) cannot be converted to an AnyValue. Note that
 * with the current implementation, this never happens.
*/
extern const ProtocolResult ClientNetworkDecodingError;
/**
 * @brief Error when the network server cannot decode the network packet into a transport packet.
 *
 * Example: the received pvxs::Value (network packet) cannot be converted to an AnyValue. Note that
 * with the current implementation, this never happens.
*/
extern const ProtocolResult ServerNetworkDecodingError;
/**
 * @brief Error when the transport client cannot encode the Protocol input into
 * a transport packet.
 *
 * @note This only happens when the Protocol input value is empty. All other values are trivially
 * supported.
*/
extern const ProtocolResult ClientTransportEncodingError;
/**
 * @brief Error when the transport server cannot encode the Protocol input or result/output into
 * a transport packet.
 *
 * @note This happens when the encapsulated Protocol of ProtocolRPCServer throws.
*/
extern const ProtocolResult ServerTransportEncodingError;
/**
 * @brief Error when the transport client cannot extract the Protocol output from the
 * transport packet.
 *
 * @note This signals a malformed transport message (reply).
*/
extern const ProtocolResult ClientTransportDecodingError;
/**
 * @brief Error when the transport server cannot extract the Protocol input from the
 * transport packet.
 *
 * @note This signals a malformed transport message (request).
*/
extern const ProtocolResult ServerTransportDecodingError;
/**
 * @brief Error when the Protocol client cannot encode its input into a protocol input.
*/
extern const ProtocolResult ClientProtocolEncodingError;
/**
 * @brief Error when the Protocol server cannot encode the application data to the protocol
 * ouput.
 *
 * @note This happens during retrieval of application protocol info, when the passed output
 * parameter has the wrong type.
*/
extern const ProtocolResult ServerProtocolEncodingError;
/**
 * @brief Error when the Protocol client cannot extract the application return value(s) from the
 * Protocol packet.
*/
extern const ProtocolResult ClientProtocolDecodingError;
/**
 * @brief Error when the Protocol server cannot extract the application input from the
 * Protocol packet.
 *
 * @note This signals a malformed Protocol message (request or reply).
*/
extern const ProtocolResult ServerProtocolDecodingError;
/**
 * @brief Error when the Protocol server cannot decode the payload from the transport packet.
 *
 * @note This signals an unsupported encoding type.
*/
extern const ProtocolResult ServerUnsupportedPayloadEncodingError;
/**
 * @brief Error when the Protocol server receives a request about an asynchronous request with an
 * unknown identifier.
 *
 * @note This error is only used in asynchronous RPC calls.
*/
extern const ProtocolResult InvalidRequestIdentifierError;
/**
 * @brief Error when the Protocol server receives a request for an asynchronous request that is
 * not valid, e.g. a client tries to get the reply when it was not ready.
*/
extern const ProtocolResult InvalidAsynchronousOperationError;
/**
 * @brief Error when the Protocol at the server side throws an exception during a call to
 * `Protocol::Invoke` or `Protocol::Service`. Note that since the Protocol implementation comes
 * from the application layer, sup-protocol has no specific way to handle such exceptions.
*/
extern const ProtocolResult ServerProtocolException;
/**
 * @brief Error when the injected transport AnyFunctor on the client side throws an exception during
 * its function call operator. Note that the transport implementation is outside of the
 * control of the protocol client.
*/
extern const ProtocolResult ClientTransportException;
/**
 * @brief Error when the Protocol server receives a request for an asynchronous request that is
 * not valid, e.g. a client tries to get the reply when it was not ready.
*/
extern const ProtocolResult AsynchronousProtocolTimeout;

}  // namespace protocol

}  // namespace sup

#endif  // SUP_PROTOCOL_PROTOCOL_RESULT_H_
