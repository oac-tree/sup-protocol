Protocol Stack
==============

`sup-protocol` provides abstractions that allow application programmers to define and implement application specific interfaces that need to be exposed as a Remote Procedure Call over the network. The implementation does not allow on any specific network protocol to avoid direct coupling between application level code and low-level network code.

.. contents::
   :local:

Architecture
------------

The following image summarizes the relationship between the main classes and interfaces involved in exposing an application specific interface as an RPC:

.. image:: /images/sup_protocol_stack.png

