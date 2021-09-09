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



6.1 DCTCP
---------------

We conclude with an example of a situation where a variant of the TCP
congestion control algorithm has been designed to work in concert with
ECN: in cloud datacenters. The combination is called DCTCP, which stands
for *Data Center TCP*. The situation is unique in that a datacenter is
self-contained, and so it is possible to deploy a tailor-made version of
TCP that does not need to worry about treating other TCP flows fairly.
Datacenters are also unique in that they are built using low-cost
white-box switches, and because there is no need to worry about long-fat
pipes spanning a continent, the switches are typically provisioned
without an excess of buffers.

The idea is straightforward. DCTCP adapts ECN by estimating the fraction
of bytes that encounter congestion rather than simply detecting that
some congestion is about to occur. At the end hosts, DCTCP then scales
the congestion window based on this estimate. The standard TCP algorithm
still kicks in should a packet actually be lost. The approach is
designed to achieve high-burst tolerance, low latency, and high
throughput with shallow-buffered switches.

The key challenge DCTCP faces is to estimate the fraction of bytes
encountering congestion. Each switch is simple. If a packet arrives and
the switch sees the queue length (K) is above some threshold; e.g.,

.. math:: \mathsf{K} > \mathsf{(RTT × C)\ /\ 7}

where C is the link rate in packets per second, then the switch sets the
CE bit in the IP header. The complexity of RED is not required.

The receiver then maintains a boolean variable for every flow, which
we’ll denote ``SeenCE``, and implements the following state machine in
response to every received packet:

-  If the CE bit is set and ``SeenCE=False``, set ``SeenCE`` to True and
   send an immediate ACK.

-  If the CE bit is not set and ``SeenCE=True``, set ``SeenCE`` to False
   and send an immediate ACK.

-  Otherwise, ignore the CE bit.

The non-obvious consequence of the “otherwise” case is that the receiver
continues to send delayed ACKs once every *n* packets, whether or not
the CE bit is set. This has proven important to maintaining high
performance.

Finally, the sender computes the fraction of bytes that encountered
congestion during the previous observation window (usually chosen to be
approximately the RTT), as the ratio of the total bytes transmitted and
the bytes acknowledged with the ECE flag set. DCTCP grows the congestion
window in exactly the same way as the standard algorithm, but it reduces
the window in proportion to how many bytes encountered congestion during
the last observation window.

6.2 LEDBAT 
-----------------

6.3 OnRamp
-----------------

6.4 QUIC
-----------------

6.5 TCP-Friendly Protocols
--------------------------
