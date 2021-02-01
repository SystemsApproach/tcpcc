Chapter 3:  Design Space
==========================

Now that we have the architectural foundation of TCP/IP in place, we
are ready to explore the design space for addressing congestion
control. To do this, we take a step back and consider the more general
problem of resource allocation, of which congestion control is a more
specific example.

In other words, the mechanisms presented in later chapters are all
trying to allocate switch queues and link bandwidth—both potentially
scarce resources—to end-to-end network flows. There is no central
"allocator" in a network the size of the Internet, so we solve the
problem in a distributed way, with all the hosts at the edge of the
network and all the switches inside of the network collaboratively
making local decisions.

This chapter scopes the design space for how these various parties
make those decisions. It does this two parts. First, it presents a
taxonomy for congestion control mechanisms. Second, it defines the
criteria by which different mechanisms can be quantitatively evaluated
and compared.

3.1 Resource Allocation
--------------------------

We start by introducing three ways to characterize resource allocation
mechanisms in general, and congestion control algorithms in
particular. This taxonomy serves to both place congestion control in
the larger context of resource allocation.


Router-Centric versus Host-Centric
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Resource allocation mechanisms can be classified into two broad
groups: those that address the problem from inside the network (i.e.,
at the routers or switches) and those that address it from the edges
of the network (i.e., in the hosts, perhaps as part of the transport
protocol).  Since it is the case that both the routers inside the
network and the hosts at the edges of the network participate in
resource allocation, the real issue is where the majority of the
burden falls.

In a router-centric design, each router takes responsibility for
deciding when packets are forwarded and selecting which packets are to
be dropped, as well as for informing the hosts that are generating the
network traffic how many packets they are allowed to send. In a
host-centric design, the end hosts observe the network conditions (e.g.,
how many packets they are successfully getting through the network) and
adjust their behavior accordingly. Note that these two groups are not
mutually exclusive. For example, a network that places the primary
burden for managing congestion on routers still expects the end hosts to
adhere to any advisory messages the routers send, while the routers in
networks that use end-to-end congestion control still have some policy,
no matter how simple, for deciding which packets to drop when their
queues do overflow.

Reservation-Based versus Feedback-Based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A second way that resource allocation mechanisms are sometimes
classified is according to whether they use *reservations* or
*feedback*. In a reservation-based system, some entity (e.g., the end
host) asks the network for a certain amount of capacity to be allocated
for a flow. Each router then allocates enough resources (buffers and/or
percentage of the link’s bandwidth) to satisfy this request. If the
request cannot be satisfied at some router, because doing so would
overcommit its resources, then the router rejects the reservation. This
is analogous to getting a busy signal when trying to make a phone call.
In a feedback-based approach, the end hosts begin sending data without
first reserving any capacity and then adjust their sending rate
according to the feedback they receive. This feedback can be either
*explicit* (i.e., a congested router sends a “please slow down” message
to the host) or *implicit* (i.e., the end host adjusts its sending rate
according to the externally observable behavior of the network, such as
packet losses).

Note that a reservation-based system always implies a router-centric
resource allocation mechanism. This is because each router is
responsible for keeping track of how much of its capacity is currently
available and deciding whether new reservations can be admitted. Routers
may also have to make sure each host lives within the reservation it
made. If a host sends data faster than it claimed it would when it made
the reservation, then that host’s packets are good candidates for
discarding, should the router become congested. On the other hand, a
feedback-based system can imply either a router- or host-centric
mechanism. Typically, if the feedback is explicit, then the router is
involved, to at least some degree, in the resource allocation scheme. If
the feedback is implicit, then almost all of the burden falls to the end
host; the routers silently drop packets when they become congested.

Window-Based versus Rate-Based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A third way to characterize resource allocation mechanisms is
according to whether they are *window based* or *rate based*. This is
one of the areas, noted above, where similar mechanisms and
terminology are used for both flow control and congestion
control. Both flow-control and resource allocation mechanisms need a
way to express, to the sender, how much data it is allowed to
transmit. There are two general ways of doing this: with a *window* or
with a *rate*. We have already seen window-based transport protocols,
such as TCP, in which the receiver advertises a window to the
sender. This window corresponds to how much buffer space the receiver
has, and it limits how much data the sender can transmit; that is, it
supports flow control. A similar mechanism—window advertisement—can be
used within the network to reserve buffer space (i.e., to support
resource allocation). TCP’s congestion-control mechanisms are window
based.

It is also possible to control a sender’s behavior using a rate—that
is, how many bits per second the receiver or network is able to
absorb.  Rate-based control makes sense for many multimedia
applications, which tend to generate data at some average rate and
which need at least some minimum throughput to be useful. For example,
a video codec might generate video at an average rate of 1 Mbps with a
peak rate of 2 Mbps.  Generally, a rate-based characterization of
flows is a logical choice in a reservation-based system that supports
different qualities of service—the sender makes a reservation for so
many bits per second, and each router along the path determines if it
can support that rate, given the other flows it has made commitments
to.

Summary
~~~~~~~~~~~~~~

Classifying resource allocation approaches along each of these three
dimensions would seem to suggest up to eight unique strategies. While
eight different approaches are certainly possible, we note that in
practice two general strategies seem to be most prevalent, both of
which are tied to the underlying service model of the network.

On the one hand, a best-effort service model usually implies that
feedback is being used, since such a model does not allow users to
reserve network capacity. This, in turn, means that most of the
responsibility for congestion control falls to the end hosts, perhaps
with some assistance from the routers. In practice, such networks use
window-based information. This is the general strategy adopted in the
Internet.

On the other hand, a QoS-based service model probably implies some
form of reservation. Support for these reservations is likely to
require significant router involvement, such as queuing packets
differently depending on the level of reserved resources they
require. Moreover, it is natural to express such reservations in terms
of rate, since windows are only indirectly related to how much
bandwidth a user needs from the network.

3.2 Control versus Avoidance 
--------------------------------

This book focuses on the first of the two scenarios just outlined: a
feedback/end-host/window-based approach, which in short, is a way to
characterize TCP congestion control as distinct from the general
problem of allocating network resources. (Chapter 6 is the exception,
where we consider how routers might assist end hosts in doing a better
job by providing more explicit feedback.)

In that context, there is an important distinction between two general
approaches to congestion control: Whether the mechanim purposely
causes packet loss and then respond to it, or if the mechanism instead
tries to prevent the queue buildup that leads to congestion in the
first place. We narrowly refer to the mechanisms of the first type (of
which Reno and CUBIC are examples) as *congestion control* algorithms,
and we refer to mechanisms of the second type as *congestion
avoidance* algorithms (of which Vegas and BBR are examples).

This distinction is often not made (and the term "congestion control"
is used generically to refer to both), but our take is that the
distinction is important and so we will call it out when appropriate.
Admittedly, we will also fall back to the generic use of "congestion
control" when the distinction is not critical to the discussion.


3.3 Evaluation Criteria
-----------------------

..
	Other quantitative measures? Stability, Persistent Queues?

Beyond characterizing space of possible resource allocation
mechanisms, there is the question of whether any given solution is
good or not. Recall that in Chapter 1 we posed the question of how a
network *effectively* and *fairly* allocates its resources. This
suggests at least two broad measures by which a resource allocation
scheme can be evaluated. We consider each in turn.


Effective Resource Allocation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A good starting point for evaluating the effectiveness of a resource
allocation scheme is to consider the two principal metrics of
networking: throughput and delay. Clearly, we want as much throughput
and as little delay as possible. Unfortunately, these goals are often
somewhat at odds with each other. One sure way for a resource allocation
algorithm to increase throughput is to allow as many packets into the
network as possible, so as to drive the utilization of all the links up
to 100%. We would do this to avoid the possibility of a link becoming
idle because an idle link necessarily hurts throughput. The problem with
this strategy is that increasing the number of packets in the network
also increases the length of the queues at each router. Longer queues,
in turn, mean packets are delayed longer in the network.

To describe this relationship, some network designers have proposed
using the ratio of throughput to delay as a metric for evaluating the
effectiveness of a resource allocation scheme. This ratio is sometimes
referred to as the *power* of the network:

::

   Power = Throughput / Delay

Note that it is not obvious that power is the right metric for judging
resource allocation effectiveness. For one thing, the theory behind
power is based on an M/M/1 queuing network that assumes infinite
queues;\ [#]_ real networks have finite buffers and sometimes have to
drop packets.  For another, power is typically defined relative to a
single connection (flow); it is not clear how it extends to multiple,
competing connections. Despite these rather severe limitations,
however, no alternatives have gained wide acceptance, and so power
continues to be used.

.. [#] Since this is not a queuing theory book, we provide only this
       brief description of an M/M/1 queue. The 1 means it has a
       single server, and the Ms mean that the distribution of both
       packet arrival and service times is *Markovian,* that is,
       exponential.

The objective is to maximize this ratio, which is a function of how
much load you place on the network. The load, in turn, is set by the
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

As we will see in later sections of this chapter, many
congestion-control schemes are able to control load in only very crude
ways; that is, it is simply not possible to turn the “knob” a little
and allow only a small number of additional packets into the
network. As a consequence, network designers need to be concerned
about what happens even when the system is operating under extremely
heavy load—that is, at the rightmost end of the curve in
:numref:`Figure %s <fig-power>`. Ideally, we would like to avoid the
situation in which the system throughput goes to zero because the
system is thrashing. In networking terminology, we want a system that
is *stable*—where packets continue to get through the network even
when the network is operating under heavy load. If a mechanism is not
stable, the network may experience *congestion collapse*.

Fair Resource Allocation
~~~~~~~~~~~~~~~~~~~~~~~~

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
  
