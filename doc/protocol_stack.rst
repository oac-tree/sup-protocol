Protocol Stack
==============

`sup-protocol` provides abstractions that allow application programmers to define and implement application specific interfaces that need to be exposed as a Remote Procedure Call (RPC) over the network. The implementation does not allow on any specific network protocol to avoid direct coupling between application level code and low-level network code.

.. contents::
   :local:

Architecture
------------

The following image summarizes the relationship between the main classes and interfaces involved in exposing an application specific interface as an RPC:

.. image:: /images/sup_protocol_stack.png

At the top of the diagram is the application specific code. This code defines an API in the `ApplicationService` interface that the programmer wishes to expose as an RPC. When the `Consumer` class interacts with this interface, it should not be able to distinguish between local implementations of this service and the `ApplicationProtocolClient` class that forwards those method calls so they can be handled by a server side implementation.

To be able to expose the `ApplicationService` as an RPC, the application developers need to implement two concrete classes:

* `ApplicationProtocolClient`: this class translates API calls from `ApplicationService` to the generic method `Protocol::Invoke`, defined as part of sup-protocol;
* `ApplicationProtocolServer`: this class does the inverse. It implements the API of `Protocol`, i.e. mainly its `Invoke` method, and translates its payload to calls on an `ApplicationService` implementation.

