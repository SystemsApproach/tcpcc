Chapter 3:  Design Space
==========================

With the architectural foundation of TCP/IP in place, we are ready to
explore the design space for addressing congestion.  But to do this,
it is helpful to first take a step back and consider the bigger
picture. The Internet is a complex arrangement of compute, storage,
and communication resources that is shared among millions of
users. The challenge is how to assign those resources—specifically
switching capacity, buffer space, and link bandwidth—to end-to-end
packet flows.

Because the Internet originally adopted a best-effort service model,
and users (or more precisely, TCP running on their behalf) were free
to send as many packets into the network as they could generate, it
was not surprising that the Internet eventually suffered from the
*tragedy of the commons*. And with congestion collapse inevitable, the
natural response was to try to control it. Hence the term *congestion
control*, which can be viewed as an implicit mechanism for allocating
resources. One that is reactive to resources becoming scarce.

A network service model in which resources are explicitly allocated to
packet flows is the obvious alternative, but given the best-effort
assumption of IP, such an approach was not viable at the time
congestion became a worrisome issue. Subsequently, work has been done
to retrofit more explict resource allocation mechanisms to the
Internet's best-effort delivery model. These include the ability to
make *Quality-of-Service (QoS)* guarantees. It is instructive to
consider the Internet's approach to congestion in the context of such
efforts. The first section does so as it explores the set of design
decisions that underly the control mechanisms outlined in this book.
The following section then defines the criteria by which different
congestion-control mechanisms can be quantitatively evaluated and
compared.

3.1 Implementation Choices
-------------------------------

We start by introducing five implementation choices that TCP
congestion control faced, and the design rationale behind the
decisions that were made. Some of the decision were "obvious" given
the circumstances under which they were made, but for completeness—and
because the Internet's continual evolution means circumstances can
change—it is prudent to consider them all.

Centralized versus Distributed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Obviously distributed, due to scale, but there are limited domains in
which centralized is feasible... e.g., B4.

Router-Centric versus Host-Centric 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given a distributed approach to resource allocation, the next question
whether is to address it inside the network (i.e., at the routers or
switches) or from the edges of the network (i.e., in the hosts,
perhaps as part of the transport protocol). This is not strictly an
either/or situation. Both locations are involved, and real issue is
where the majority of the burden falls. On the one hand, individual
routers always take responsibility for deciding when packets are
forwarded and selecting which packets are to be dropped. On the other
hand, there is a range of options for how the router interacts with
hosts, and hence, how much responsibility the host takes on.

In a router-centric design, routers can go so far as to reserve
capacity and ensure each flow's packets are delivered accordingly. In
a host-centric design, the router makes no guarantees and provides
minimal feedback about the available capacity, and it is the host's
responsibility to observe the network conditions (e.g., how many
packets they are successfully getting through the network) and adjust
its behavior accordingly. How much is minimal? The absolute least a
router can do is silently drop a packet when its buffers are full, but
as we will see in Chapter 7, there are more proactive actions routers
can take to assist the end hosts in doing their job.

Reservation-Based versus Feedback-Based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Another way of asking the host-centric versus router-center question
is whether the allocation mechanism uses *reservations* or *feedback*.
In a reservation-based system, the end host asks the network for a
certain amount of capacity to be allocated for a flow.  Each router
then allocates enough resources (buffers and/or percentage of the
link’s bandwidth) to satisfy this request. If the request cannot be
satisfied at some router, because doing so would overcommit its
resources, then the router rejects the reservation. This is exactly
how a system that makes QoS guarantees is implemented.

In a feedback-based approach, the end hosts begin sending data without
first reserving any capacity and then adjust their sending rate
according to the feedback they receive. This feedback can be either
*explicit* (i.e., a congested router sends a “please slow down”
message to the host) or *implicit* (i.e., the end host adjusts its
sending rate according to the externally observable behavior of the
network, such as packet losses).

A reservation-based system always implies a router-centric resource
allocation mechanism. This is because each router is responsible for
keeping track of how much of its capacity is currently available and
deciding whether new reservations can be admitted.  Routers may also
have to make sure each host lives within the reservation it made; this
is sometimes called *policing*. On the other hand, a feedback-based
system can imply either a router- or host-centric
mechanism. Typically, if the feedback is explicit, then the router is
involved, to at least some degree, in the resource allocation
scheme. If the feedback is implicit, then almost all of the burden
falls to the end host; the routers silently drop packets when they
become congested.

Window-Based versus Rate-Based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Another implementation choice is whether it is *window based* or *rate
based*. This is one of the areas where similar mechanisms and
terminology are used for both flow control and congestion control,
both of which need a way to express how much data the sender is
allowed to transmit. There are two general ways of doing this: with a
*window* or with a *rate*.

Rate-based control makes sense for many multimedia applications, which
tend to generate data at some average rate and which need at least
some minimum throughput to be useful. For example, a video codec might
generate video at an average rate of 1 Mbps with a peak rate of
2 Mbps.  Generally, a rate-based scheme is a logical choice in a
reservation-based system that supports different qualities of
service—the sender makes a reservation for so many bits-per-second,
and each router along the path determines if it can support that rate,
given the other flows it has made commitments to.

TCP uses window-based mechanism to implement flow control, so the
design decision for TCP congestion control seems obvious.  And in
fact, many of the congestion-control mechanisms described in this book
are window-based. But is also possible for a congestion control
mechanism to compute the rate at which the network is able to deliver
packets based on implicit or explicit feedback, and to pace the
transmission of packets accordingly.  Observed rate is just the number
of bytes delivered over some time period, such as a window's worth of
data in one RTT.  We will see example mechanims do exactly that in
later chapters.

Control-based versus Avoidance-based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final implementation choice we draw attention to somewhat subtle.
The challenge is for the the end-host, based on feedback, to compute
how much capacity is available in the network, and throttle its
sending rate accordingly. There are two general strategies for doing
this: an aggresive approach that purposely causes packet loss and then
responds to it, and a conservative approach that tries to detect the
onset of router queues building up and slowing down before the queues
actually overflow.  We narrowly refer to the mechanisms of the first
type as *congestion-based*, and we refer to mechanisms of the second
type as *avoidance-based*.

This distinction is often overlooked—and the term "congestion control"
is used generically to refer to both—but our take is that the
distinction represents and important difference, and so we will call
it out when appropriate.  Admittedly, we will also fall back to the
generic use of "congestion control" when the distinction is not
critical to the discussion, but we will say "congestion-based" when
the distinction is relevant.

3.2 Evaluation Criteria
-----------------------

..
	Other quantitative measures? Stability, Persistent Queues?

Having identified the set of design decisions that go into crafting a
congestion-control mechanism, the next question is whether any given
solution is good or not. Recall that in Chapter 1 we posed the
question of how a network *effectively* and *fairly* allocates its
resources. This suggests at least two broad measures by which a
resource allocation scheme can be evaluated. We consider each in turn.

Effectiveness
~~~~~~~~~~~~~

A good starting point for evaluating the effectiveness of a
congestion-control mechanism is to consider the two principal metrics
of networking: throughput and delay. Clearly, we want as much
throughput and as little delay as possible. Unfortunately, these goals
are often somewhat at odds with each other. One sure way for a
resource allocation algorithm to increase throughput is to allow as
many packets into the network as possible, so as to drive the
utilization of all the links up to 100%. We would do this to avoid the
possibility of a link becoming idle because an idle link necessarily
hurts throughput. The problem with this strategy is that increasing
the number of packets in the network also increases the length of the
queues at each router. Longer queues, in turn, mean packets are
delayed longer in the network.

The ratio of throughput to delay as a general and widely-accepted
metric for evaluating the effectiveness of a resource allocation
scheme. This ratio is sometimes referred to as the *power* of the
system:

::

   Power = Throughput / Delay

The objective is to maximize this ratio, which is a function of how
much load you place on the system. The load, in turn, is set by the
resource allocation mechanism. :numref:`Figure %s <fig-power>` gives a
representative power curve, where, ideally, the resource allocation
mechanism would operate at the peak of this curve. To the left of the
peak, the mechanism is being too conservative; that is, it is not
allowing enough packets to be sent to keep the links busy. To the
right of the peak, so many packets are being allowed into the network
that increases in delay due to queuing are starting to dominate any
small gains in throughput.

Interestingly, this power curve looks very much like the system
throughput curve in a timesharing computer system. System throughput
improves as more jobs are admitted into the system, until it reaches a
point when there are so many jobs running that the system begins to
thrash (spends all of its time swapping memory pages) and the throughput
begins to drop.
   
.. _fig-power:
.. figure:: figures/f06-03-9780123850591.png
   :width: 350px
   :align: center

   Ratio of throughput to delay as a function of load.

As we will see in later chapters, many congestion-control schemes are
able to control load in only very crude ways; that is, it is simply
not possible to turn the “knob” a little and allow only a small number
of additional packets into the network. As a consequence, network
designers need to be concerned about what happens even when the system
is operating under extremely heavy load—that is, at the rightmost end
of the curve in :numref:`Figure %s <fig-power>`. Ideally, we would
like to avoid the situation in which the system throughput goes to
zero because the system is thrashing. In networking terminology, we
want a system that is *stable*—where packets continue to get through
the network even when the network is operating under heavy load. If a
mechanism is not stable, the network may experience *congestion
collapse*.

Fairness
~~~~~~~~~~~~~

..
	Current cut-and-paste is largely based on Jain’s work. Need to
	also include the latest work from Ware’s thesis at CMU:
	https://www.cs.cmu.edu/~rware/assets/pdf/ware-hotnets19.pdf

The effective utilization of network resources is not the only criterion
for judging a resource allocation scheme. We must also consider the
issue of fairness. However, we quickly get into murky waters when we try
to define what exactly constitutes fair resource allocation. For
example, a reservation-based resource allocation scheme provides an
explicit way to create controlled unfairness. With such a scheme, we
might use reservations to enable a video stream to receive 1 Mbps across
some link while a file transfer receives only 10 kbps over the same
link.

In the absence of explicit information to the contrary, when several
flows share a particular link, we would like for each flow to receive
an equal share of the bandwidth. This definition presumes that a
*fair* share of bandwidth means an *equal* share of bandwidth. But,
even in the absence of reservations, equal shares may not equate to
fair shares.  Should we also consider the length of the paths being
compared? For example, as illustrated in :numref:`Figure %s
<fig-path-len>`, what is fair when one four-hop flow is competing with
three one-hop flows?
   
.. _fig-path-len:
.. figure:: figures/f06-04-9780123850591.png
   :width: 600px
   :align: center

   One four-hop flow competing with three one-hop flows.

Assuming that fair implies equal and that all paths are of equal length,
networking researcher Raj Jain proposed a metric that can be used to
quantify the fairness of a congestion-control mechanism. Jain’s fairness
index is defined as follows. Given a set of flow throughputs

.. math::

   (x_{1}, x_{2}, \ldots , x_{n})

(measured in consistent units such as bits/second), the following
function assigns a fairness index to the flows:

.. math::

   f(x_{1}, x_{2}, \ldots ,x_{n}) = \frac{( \sum_{i=1}^{n} x_{i}
   )^{2}} {n  \sum_{i=1}^{n} x_{i}^{2}}

The fairness index always results in a number between 0 and 1, with 1
representing greatest fairness. To understand the intuition behind this
metric, consider the case where all *n* flows receive a throughput of
1 unit of data per second. We can see that the fairness index in this
case is

.. math::

   \frac{n^2}{n \times n} = 1

Now, suppose one flow receives a throughput of :math:`1 + \Delta`. 
Now the fairness index is

.. math::

   \frac{((n - 1) + 1 + \Delta)^2}{n(n - 1 + (1 + \Delta)^2)}
   = \frac{n^2 + 2n\Delta + \Delta^2}{n^2 + 2n\Delta + n\Delta^2}

Note that the denominator exceeds the numerator by :math:`(n-1)\Delta^2`.
Thus, whether the odd flow out was getting more or less than all the
other flows (positive or negative :math:`\Delta`), the fairness index has 
now dropped below one. Another simple case to
consider is where only *k* of the *n* flows receive equal throughput,
and the remaining *n-k* users receive zero throughput, in which case the
fairness index drops to \ *k/n*.
  
