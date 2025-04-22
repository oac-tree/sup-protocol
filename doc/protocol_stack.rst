Protocol Stack
==============

`sup-protocol` provides abstractions that allow application programmers to define and implement application specific interfaces that need to be exposed as a Remote Procedure Call (RPC) over the network. The implementation does not rely on any specific network protocol to avoid direct coupling between application level code and low-level network code.

.. contents::
   :local:

Architecture
------------

The following image summarizes the relationship between the main classes and interfaces involved in exposing an application specific interface as an RPC:

.. image:: /images/sup_protocol_stack.png

At the top of the diagram is the application specific code. This code defines an API in the `ApplicationService` interface that the programmer wishes to expose as an RPC. When the `Consumer` class interacts with this interface, it should not be able to distinguish between local implementations of this service (`ConcreteService`) and the `ApplicationProtocolClient` class that forwards method calls so they can be handled by a server side implementation.

To be able to expose the `ApplicationService` as an RPC, the application developers need to implement two concrete classes:

* `ApplicationProtocolClient`: this class translates API calls from `ApplicationService` to the generic method `Protocol::Invoke`, defined as part of sup-protocol;
* `ApplicationProtocolServer`: this class does the inverse. It implements the API of `Protocol`, i.e. mainly its `Invoke` method, and translates its payload to calls on an `ApplicationService` implementation.

The `Protocol` interface contains the following methods:

.. function:: ProtocolResult Invoke(const AnyValue& input, AnyValue& output)

   :return: ProtocolResult indicating success or failure conditions.

   The main API method that provides access to the application layer over the network.

.. function:: ProtocolResult Service(const AnyValue& input, AnyValue& output)

   :return: ProtocolResult indicating success or failure conditions.

   API method that can provide generic application protocol information to the caller.

The following basic `ProtocolResult` objects are defined:

.. list-table::
   :widths: 30 85
   :header-rows: 1

   * - ProtocolResult
     - Meaning
   * - Success
     - Successful call
   * - NotConnected
     - Error in the exchange of messages between client and server at the network layer
   * - ClientNetworkEncodingError
     - Error when the network client cannot encode the transport packet into a network packet
   * - ServerNetworkEncodingError
     - Error when the network server cannot encode the transport packet into a network packet
   * - ClientNetworkDecodingError
     - Error when the network client cannot decode the network packet into a transport packet
   * - ServerNetworkDecodingError
     - Error when the network server cannot decode the network packet into a transport packet
   * - ClientTransportEncodingError
     - Error when the transport client cannot encode the Protocol input into a transport packet
   * - ServerTransportEncodingError
     - Error when the transport server cannot encode the Protocol input or result/output into a transport packet
   * - ClientTransportDecodingError
     - Error when the transport client cannot extract the Protocol output from the transport packet
   * - ServerTransportDecodingError
     - Error when the transport server cannot extract the Protocol input from the transport packet
   * - ClientProtocolEncodingError
     - Error when the Protocol client cannot encode its input into a protocol input
   * - ServerProtocolEncodingError
     - Error when the Protocol server cannot encode the application data to the protocol ouput
   * - ClientProtocolDecodingError
     - Error when the Protocol client cannot extract the application return value(s) from the protocol packet
   * - ServerProtocolDecodingError
     - Error when the Protocol server cannot extract the application input from the protocol packet
   * - ServerUnsupportedPayloadEncodingError
     - Error when the Protocol server cannot decode the payload from the transport packet
   * - InvalidRequestIdentifierError
     - Error when the Protocol server receives a request about an asynchronous request with an unknown identifier
   * - InvalidAsynchronousOperationError
     - Error when the Protocol server receives a request for an asynchronous request that is not valid
   * - ServerProtocolException
     - Error when the Protocol at the server side throws an exception
   * - ClientTransportException
     - Error when the injected transport AnyFunctor on the client side throws an exception
   * - AsynchronousProtocolTimeout
     - Error when an asynchronous request times out
