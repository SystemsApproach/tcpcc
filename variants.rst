Chapter 6:  TCP Variants and Alternatives
=========================================

As exploration of the design space for congestion control has
continued, a number of new protocols that are designed for specific
environments have emerged. These differ from TCP mostly in that they
have targeted specific use cases, rather than the arbitrarily complex
and heterogeneous network environments that TCP supports. The
exception may be QUIC, which started with the goal of improving HTTP
performance specifically, but has now developed into something of a general
TCP alternative.

This chapter is not exhaustive, as new approaches to congestion
control are being invented all the time. Instead we survey a few
approaches that mark different points in the design space.
Data Center TCP (DCTCP) draws on realizations that data centers are dominated by east-west
(or intra-data-center) traffic and that low latency is a requirement
for many applications in the DC, leading to a design optimized for that
environment. Many further attempts to improve congestion behavior in data
center environments followed.

LEDBAT (Low Extra Delay Background Transport) targets a completely
difference environment and use case: transferring large amounts of
data across the Internet without greatly increasing the latency of other
competing TCP connections.

On-Ramp focuses on yet another part of the design space: transient
congestion in public clouds. Again, a different set of constraints
leads to a different point in the design space.

Finally we look at the space of "TCP-friendly" congestion control,
where the working assumption is that some applications—notably those
involving real-time communication—can't directly
use the reliable byte-stream model of TCP, and hence need a different
sort of transport. At the same time, the sound operation of the
Internet requires that these flows respond to congestion in a way that
is sufficiently similar to TCP that actual TCP flows are not pushed aside.



6.1 Data Center TCP (DCTCP)
---------------------------

*Data Center TCP* was one of the first TCP
variants designed specifically to perform well in cloud data centers. There are
several aspects of the data center environment that warrant an
approach that differs from more traditional TCP. These include:

* Round trip time for intra-DC traffic are small;
  
* Buffers in data center switches are also typically small;
  
* All the switches are under common administrative control, and thus
  can be required to meet certain standards;
  
* A great deal of traffic has low latency requirements;
  
* That traffic competes with high bandwidth flows.

It should be noted that DCTCP is not just a version of TCP, but rather
a system design that changes both the switch behavior and the end host
response to congestion information received from switches.  

The central insight in DCTCP is that using loss as the main signal of
congestion in the data center environment is insufficient. By the time a queue
has built up enough to overflow, low latency traffic is already failing
to meet its deadlines, negatively impacting performance. Thus DCTCP
uses a version of ECN to provide an early signal of congestion. But
whereas the original design of ECN treated an ECN marking much like a
dropped packet, and cut the congestion window in half, DCTCP takes a
more finely-tuned approach. DCTCP tries to estimate the fraction
of bytes that are encountering congestion rather than making the simple
binary decision that congestion is present. It then scales
the congestion window based on this estimate. The standard TCP algorithm
still kicks in should a packet actually be lost. The approach is
designed to keep queues short by reacting early to congestion while
not over-reacting to the point that they run empty and sacrifice
throughput. 

The key challenge in this approach is to estimate the fraction of bytes
encountering congestion. Each switch is simple. If a packet arrives and
the switch sees the queue length (K) is above some threshold; e.g.,

.. math:: \mathsf{K} > \mathsf{(RTT × C)\ /\ 7}

where C is the link rate in packets per second, then the switch sets the
CE bit in the IP header. The complexity of RED is not required.

The receiver then maintains a boolean variable for every flow, which
we’ll denote ``DCTCP.CE``, and sets it initially to false. When sending
an ACK, the receiver sets the ECE (Echo Congestion Experienced) flag
in the TCP header if and only if ``DCTCP.CE`` is true. It also
implements the following state machine in response to every received
packet:

-  If the CE bit is set and ``DCTCP.CE=False``, set ``DCTCP.CE`` to True and
   send an immediate ACK.

-  If the CE bit is not set and ``DCTCP.CE=True``, set ``DCTCP.CE`` to False
   and send an immediate ACK.

-  Otherwise, ignore the CE bit.

The non-obvious consequence of the “otherwise” case is that the
receiver continues to send delayed ACKs once every *n* packets, as
long as a stream of packets with a constant CE value is
received. Delayed ACKs have proven important to maintaining high
performance.

At the end of each observation window (a period usually chosen to be
approximately the RTT), the sender computes the fraction of bytes that
encountered congestion during that window as the
ratio of the bytes marked with CE to total bytes transmitted. DCTCP
grows the congestion window in exactly the 
same way as the standard algorithm, but it reduces the window in
proportion to how many bytes encountered congestion during the last
observation window.

Specifically, a new variable called ``DCTCP.Alpha`` is initialized to
1 and updated at the end of the observation window as follows:

.. math:: \mathsf{DCTCP.Alpha} = \mathsf{DCTCP.Alpha × (1 - g) + g × M}

M is the faction of bytes marked, and g is the estimation gain, a
constant (set by the implementation) that determines how rapidly
``DCTCP.Alpha`` changes in response to marking of packets. When there
is sustained congestion, ``DCTCP.Alpha`` approaches 1, and when there
is sustained lack of congestion, ``DCTCP.Alpha`` decays to zero. This
causes gentle reaction to newly arrived congestion and more severe
reaction to sustained congestion, as the congestion window is calculated
as follows:

.. math:: \mathsf{cwnd} = \mathsf{cwnd × (1 - DCTCP.Alpha / 2)}

To summarize, CE marking to indicate incipient congestion happens
early and often, but the reaction to such marking is more measured
than in standard TCP, to avoid the over-reaction that would lead to
queues running empty.

The paper that lays out all the arguments for DCTCP including a study
of the data center traffic characteristics that motivated its design
is a "test of time" award winner from SIGCOMM.

.. _reading_dctcp:
.. admonition::  Further Reading

   Alizadeh, M., et al. `Data
   Center TCP (DCTCP)
   <http://dl.acm.org/citation.cfm?doid=1851182.1851192>`__.  
   ACM SIGCOMM, August 2010.



6.2 LEDBAT 
-----------------

6.3 OnRamp
-----------------

6.4 QUIC
-----------------

6.5 TCP-Friendly Protocols
--------------------------
