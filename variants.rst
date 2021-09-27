Chapter 7:  Beyond TCP
======================

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

.. On-Ramp focuses on yet another part of the design space: transient
   congestion in public clouds. Again, a different set of constraints
   leads to a different point in the design space.

Finally we look at the space of "TCP-friendly" congestion control,
where the working assumption is that some applications—notably those
involving real-time communication—can't directly
use the reliable byte-stream model of TCP, and hence need a different
sort of transport. At the same time, the sound operation of the
Internet requires that these flows respond to congestion in a way that
is sufficiently similar to TCP that actual TCP flows are not pushed aside.



7.1 Data Centers (DCTCP)
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



7.2 Background Transport (LEDBAT)
----------------------------------

In sharp contrast to low-latency data center environments, there are many
applications that just need to transfer a large amount of data over an
extended period of time. File-sharing protocols such as BitTorrent and
software-updates are two examples. LEDBAT (Low Extra Delay Background
Transport) is targeted at these applications.

One of the common themes among various efforts to improve TCP's
congestion control algorithm has been the idea of co-existence with
standard TCP. It is well-known that an algorithm could "outperform"
TCP by simply being more aggressive in its response to
congestion. Hence, there is an implicit assumption that new congestion
control algorithms should be evaluated alongside standard TCP to ensure
they are not just stealing bandwidth from less aggressive TCP
implementations.

LEDBAT takes this idea in a the opposite direction by creating a
congestion control protocol that is *less* aggressive than TCP. The
idea is to take advantage of bandwidth that is available when links
are uncongested, but to quickly back off and leave the bandwidth free
for other, standard flows when they arrive. In addition, as the name
suggests, LEDBAT tries not to create significant queuing delays, unlike
the typical behavior of TCP when filling a bottleneck link.

Like TCP Vegas, LEDBAT aims to detect the onset of congestion before
it is severe enough to cause loss. However, LEDBAT takes a different
approach to making this determination, using one-way measurements of delay as
the primary input to the process. This is a relatively novel approach
that makes sense in an era where reasonably accurate but not perfectly
synchronized clocks are assumed to be the norm.

To calculate one-way delay, the sender puts a timestamp in each
transmitted packet, and the receiver compares this against local
system time to measure the delay experienced by the packet. It then
sends this calculated value back to the sender. Even though the clocks
are not precisely synchronized, *changes* in this delay can be used to
infer the buildup of queues. It is assumed that the clocks do not have
large relative "skew", i.e., their relative offset does not change too quickly, which
is a reasonable assumption in practice.

The sender monitors the measured delay, and estimates the fixed
component (which would be due to speed of light and other fixed
delays) to be the lowest value seen over a certain (configurable) time
interval. Estimates from the more distant past are eliminated
to allow for the possibility of a new routing path changing the fixed delay.  Any delay larger than this 
minimum is assumed to be due to queuing delay.

Having established a "base" delay, the sender subtracts this from the
measured delay to obtain the queuing delay, and optionally uses a
filtering algorithm to reduce short-term noise in the estimate. This
estimated queuing delay is then compared to a target delay. When the delay is below target, the
congestion window is allowed to grow, and when the delay is above
target, the congestion window is reduced, with the rate of growth and
decrease being proportional to the distance from the target. The
growth rate is capped to be no faster than the growth of standard
TCP's window in its additive increase phase. 

LEDBAT's algorithm for setting the congestion window ``cwnd`` when an
ACK is received can be summarized as follows:

.. math:: \mathsf{cwnd}\  = \mathsf{cwnd + (GAIN × off\_target × bytes\_newly\_acked × MSS / cwnd)}

where GAIN is a configuration parameter between 0 and 1, off\_target is
the gap between the measured queuing delay and the target, expressed
as a fraction of the target, and bytes\_newly\_acked is the number of
bytes acknowledged in the current ACK. Thus, the congestion window
grows more quickly the further the measured delay is below the target, but never
faster one MSS per RTT. And it falls faster in proportion to how far the queue length is
above the target. ``cwnd`` is also reduced in response to losses,
timeouts, and long idle periods, much like with TCP.

Hence, LEDBAT can do a good job of using available bandwidth that is
free, but avoids creating long standing queues, as it aims to keep the
delay around the target (which is a configurable number, suggested to
be on the order of 100ms). If other traffic starts to compete with
LEDBAT traffic, LEDBAT will back off as it aims to prevent the queue getting
longer. 

LEDBAT is defined as an experimental protocol by the IETF, and allows
a considerable degree of implementation flexibility such as the choice
of filtering on delay estimates and a range of configuration
parameters. Further details can be found in the RFC.


.. _reading_ledbat:
.. admonition::  Further Reading

   Shalunov, S., et al. `Low Extra Delay Background Transport (LEDBAT)
   <https://www.rfc-editor.org/info/rfc6817>`__.  
   RFC 6817, December 2012.



.. for another day
   7.3 Public Cloud (OnRamp)
   -------------------------

7.3 HTTP Performance (QUIC)
---------------------------


QUIC originated at Google in 2012 and was subsequently developed as a
proposed standard at the IETF. It has already seen a moderate amount
of deployment (in some Web browsers and quite a number of popular Web
sites). Deployability was a key consideration for the designers of the
protocol.

The primary motivation for QUIC was to improve the performance of web
traffic compared to running HTTP over TCP. TCP turned out to be non-optimal
in a number of dimensions when HTTP was layered on top of it. These issues became more
noticeable over time, due to factors such as the rise of high-latency
wireless networks, the availability of multiple networks for a single
device (e.g., Wi-Fi and cellular), and the increasing use of
encrypted, authenticated connections on the Web.
In seeking to address these issues, QUIC required a significant
rethinking of the design of a reliable transport protocol.

Like TCP, QUIC builds congestion control into the transport, but it
does so in a way that recognizes that there is no single perfect
congestion control algorithm. Instead, there is an assumption that
different senders may use different algorithms. The baseline algorithm
in the QUIC specification is similar to TCP NewReno, but a sender can
unilaterally choose a different algorithm to use, such as CUBIC. QUIC
provides all the machinery to detect lost packets in support of
various congestion control algorithms. 

A number of design features of QUIC make the detection of loss and
congestion more robust than in TCP. For example, whereas TCP uses the
same sequence number for a packet whether it is being sent for the
first time or retransmitted, QUIC sequence numbers (called packet
numbers) are strictly increasing. A higher packet number signifies
that the packet was sent later, and a lower packet number signifies
that the packet was sent earlier. This means that it is always
possible to distinguish between a packet that has been transmitted for
the first time and one that has been retransmitted due to a loss or
timeout.

Note also that whereas TCP sequence numbers refer to bytes in the
transmitted byte stream, QUIC packet numbers refer to entire
packets. The packet number space for QUIC is large enough to avoid
wraparound issues (up to 2^62 - 1).

QUIC builds selective acknowledgments into the protocol, with support
for more than the three ranges of packets that can be acknowledged in
the TCP SACK option. This improves performance in high loss
environments, enabling forward progress to be made as long as some
packets are getting received successfully.

QUIC adopts a more robust approach to determining
packet loss than the duplicate ACKs on which TCP Fast Recovery relies. The approach was
developed independent of QUIC under the name RACK-TLP: Recent
Acknowledgments and Tail Loss Probes. A key insight is that
duplicate ACKs fail to trigger loss recovery when the sender doesn't
send enough data after the lost packet to trigger the duplicate ACKs,
or when retransmitted packets are themselves lost. Conversely, packet
reordering may also trigger fast recovery when in fact no packets have
been lost. QUIC takes the ideas of RACK-TLP to address this by using a
pair of mechanisms:

- A packet is considered lost if a packet with a higher number has
  been acknowledged, and the packet was sent "long enough in the
  past" or K packets before the acknowledged packet (K is a
  parameter).

- Probe packets are sent after waiting a "probe timeout interval" for an ACK to
  arrive, in an effort to trigger an ACK that will provide information
  about lost packets.

The first bullet ensures that modest amounts of packet reordering
are not interpreted as loss events. K is recommended to be initially
set to 3, but can be updated if there is evidence of greater
misordering. And the definition of "long enough in the past" is a
little more than the measured RTT.

The second bullet ensures that, even if
duplicate ACKs are not generated by data packets,  probe
packets are sent to elicit further ACKs, thus exposing gaps in the
received packet stream. The "probe timeout
interval" is calculated to be just long enough to account for all the
delays that an ACK might have encountered, using both the estimated RTT
and an estimate of its variance. 

QUIC is a most interesting development in the world of transport
protocols. Many of the limitations of TCP have been known for decades,
but QUIC represents one of the most successful efforts to date to
stake out a different point in the design space. It has also 
build in decades worth of experience refining TCP congestion control
into the baseline specification. Because QUIC was
inspired by experience with HTTP and the Web—which arose long after
TCP was well established in the Internet—it presents a fascinating
case study in the unforeseen consequences of layered designs and in
the evolution of the Internet. There is a lot more to it that we can
cover here. The definitive reference for QUIC is RFC 9000, but
congestion control is covered in the separate RFC 9002.


.. _reading_quic:
.. admonition::  Further Reading

   Iyengar, J.  and Swett, I., Eds.
   `QUIC Loss Detection and Congestion Control
   <https://www.rfc-editor.org/info/rfc9002>`__.  
   RFC 9002, May 2021.



7.4 TCP-Friendly Protocols (TFRC)
---------------------------------
As noted at various points throughout this book, it is easy to make
transport protocols that out-perform TCP, since TCP in all its forms
backs off when it detects congestion. Any protocol which does *not*
respond to congestion with a reduction in sending rate will eventually
get a bigger share of the bottleneck link than any TCP or TCP-like
traffic that it competes against. In the limit, this would likely lead
back to the congestion collapse that was starting to become common
when TCP congestion control was first developed. Hence, there is a
strong interest in making sure that the vast majority of traffic on
the Internet is in some sense "TCP-friendly".

When we use the term "TCP-friendly" we are saying that we expect a
similar congestion response to that of TCP. LEDBAT could be considered
"more than TCP-friendly" in the sense that it backs off even more
aggressively to congestion than TCP by reducing its window size at the
first hint of delay. But there is a class of applications for which
being TCP-friendly requires a bit more thought because they do not use
a window-based congestion scheme. These are typically "real time"
applications involving streaming multimedia.

Multimedia applications such as video streaming and telephony can
adjust their sending rate by changing coding parameters, with a
trade-off between bandwidth and quality. However, they cannot suddenly
reduce sending rate by a large amount without a perceptible impact on
the quality, and they generally need to choose among a finite set of
quality levels. These considerations lead to rate-based approaches
rather than window-based, as discussed in Section 3.1.

The approach to TCP-friendliness for these applications is to try to
pick a sending rate similar to that which would be achieved by TCP
under similar conditions, but to do so in a way that keeps the rate
from fluctuating too wildly. Underpinning this idea is a body of
research going back many years on modelling the throughput of TCP. A
simplified version of the TCP throughput equation is given in RFC 5348
which defines the standard for TFRC. With a few variables set to
recommended variables, the equation for target transmit rate X in
bits/sec is:

.. math::

   \mathsf{X} = \frac{s}{R\times\sqrt{2p/3} + 12\sqrt{3p/8}\times p
   \times (1 + 32 p^2)}

Where:

- *s* is the segment size (excluding IP and transport headers);
- *R* is the RTT in seconds;
- *p* is the number of "loss events" as a fraction of packets
  transmitted.

While the derivation of this formula is interesting in its own right
(see the second reference below),
the key idea here is that we have a pretty good idea of how much
bandwidth a TCP connection will be able to deliver if we know the RTT
and the loss rate of the path. So TFRC tries to steer applications
that cannot implement a window-based congestion control algorithm to
arrive at the same throughput as TCP would under the same conditions.

The only issues remaining to be addressed are the measurement of *p*
and *R*, and then deciding how the application should respond to
changes in *X*. Like some of the other protocols we have seen, TFRC
uses timestamps to measure RTT more accurately than TCP
originally did. Packet sequence numbers are used to determine packet
loss at the receiver, with consecutive losses grouped into a single
loss event. From this information the loss event rate *p* can be
calculated at the receiver who then reflects it back to the sender.

Exactly how the application responds to a change in rate will of
course depend on the application. The basic idea would be that an
application can choose among a set of coding rates, and it picks the
highest quality that can be accommodated with the rate that TFRC
dictates. 

An interesting footnote to the story of TFRC is that a great deal of
video streaming traffic today uses a different approach referred to as
"DASH" (Dynamic Adaptive Streaming over HTTP). DASH lets TCP (or
potentially QUIC) take care
of congestion control; the application measures the
throughput that TCP is delivering, then adjusts the quality of the
video stream accordingly to avoid starvation at the receiver. This
approach has proven to be suitable for video entertainment, but since
it depends on a moderately large amount of buffering at the receiver
to smooth out the fluctuations in TCP throughput, it is not really
suitable for interactive audio or video.


The specification of TFRC gives useful background on the design and
goes into considerable detail on how best to implement a TCP-friendly protocol.

.. _reading_tfrc:
.. admonition::  Further Reading

   Floyd, S., Handley, M., Padhye, J. and Widmer, J.
   `TCP Friendly Rate Control (TFRC): Protocol Specification
   <https://www.rfc-editor.org/info/rfc5348>`__.  
   RFC 5348, September 2008.

   
.. _reading_tcpeq:
.. admonition::  Further Reading

   Padhye, J., Firoiu, V., Towsley, D. and Kurose, J.
   `Modeling TCP Throughput: A Simple Model and its Empirical Validation
   <https://conferences.sigcomm.org/sigcomm/1998/tp/paper25.pdf>`__.  
   ACM SIGCOMM, September 1998.

   
