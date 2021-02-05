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
*tragedy of the commons*. And with starting to experience congestion
collapse, the natural response was to try to control it. Hence the
term *congestion control*, which can be viewed as an implicit
mechanism for allocating resources. One that is reactive to resources
becoming scarce.

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

We start by introducing four implementation choices that a congestion
control mechanism faces, and the design rationale behind the decisions
that were made for TCP/IP. Some of the decision were "obvious" given
the circumstances under which they were made, but for completeness—and
because the Internet's continual evolution means circumstances
change—it is prudent to consider them all.

Centralized versus Distributed
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In principle, the first design decision is whether a network's
approach to resource allocation is centralized or distributed. In
practice, the Internet's scale—along with the autonomy of the
organizations that connect to it—dictated a distributed approach.  But
acknowledging this default decision is important for two reasons.

First, while the Internet's approach to congestion control is
distributed across its millions of hosts and routers, it is fair to
think of them as cooperatively trying to achieve a globally optimal
solution.  From this perspective, there is a shared objective
function, and all the elements are implementing a distributed
algorithm to optimize that function. The various mechanisms described
throughout this book are simply defining different objective
functions, where a persistent challenge has been how to think about
competing objective functions when multiple mechanisms have been
deployed.

Second, while a centralized approach is not practical for the Internet
as a whole, it can be appropriate for limited domains. For example, a
logically centralized controller could collect information about the
state of the network's links and switches, compute a globally optimal
solution, and then advise (or even police) end hosts as to how much
capacity is available to each them. Such an approach would certainly
be limited by the time-scale in which the centralized controller could
be responsive to changes in the network, but it has been successfully
applied to the coarse-grain allocations decisions made by traffic
engineering mechanisms like B4 and SWAN.  Exactly where one draws a
line between coarse-grain traffic engineering decisions and fine-grain
congestion control decisions is not clear, but it's good to keep an
open mind about the spectrum of options that are available.

.. _reading_b4:
.. admonition:: Further Reading 

   S. Jain, et. al. `B4: Experience with a 
   Globally-Deployed Software Defined WAN 
   <https://cseweb.ucsd.edu/~vahdat/papers/b4-sigcomm13.pdf>`__.
   ACM SIGCOMM, August 2013.

Router-Centric versus Host-Centric 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given a distributed approach to resource allocation, the next question
whether is to address it inside the network (i.e., at the routers or
switches) or from the edges of the network (i.e., in the hosts,
perhaps as part of the transport protocol). This is not strictly an
either/or situation. Both locations are involved, and real issue is
where the majority of the burden falls. Individual routers always take
responsibility for deciding when packets are forwarded and selecting
which packets are to be dropped. Where there is a range of options is
how far the router goes to interact with hosts, and hence, how much
responsibility the host takes on.

At one end of the spectrum, routers can go so far as to reserve
capacity and ensure each flow's packets are delivered accordingly.  It
might do this, for example, by implementing Fair Queing, accepting new
flows only when there is sufficient capacity, and policing hosts to
make sure their flows stay within their reservations. This would
correspond to a reservation-based approach in which the network is
able to make QoS guarantees. We consider this out-of-scope for the
purpose of this book.

At the other end of the spectrum, the router makes no guarantees and
offers no feedback about the available capacity (i.e., silently drops
packets when its buffers are full) and it is the host's responsibility
to observe the network conditions (e.g., how many packets they are
successfully getting through the network) and adjust its behavior
accordingly.

In the middle of the spectrum, routers can take more proactive action
to assist the end hosts in doing their job, but not by reserving
buffer space. Instead, this involves sending *feedback* to the end
hosts when its buffers are full. We describe some of these forms of
*Active Queue Management (AQM)* in Chapter 7, but the host-centric
congestion-control mechanisms described in the next three chapters
assume routers silently taildrop packets when their buffers are full.

Window-Based versus Rate-Based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Having settled on a host-centric approach, the next implementation
choice is whether the mechanism is *window based* or *rate based*.
This is a situation where similar mechanisms and terminology are used
for flow control and congestion control, both of which need a way to
express how much data the sender is allowed to transmit. 

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
fact, the congestion-control mechanisms described in Chapter 4 are
centered around an algorithm for computing a *congestion window*,
where the sender is throttled by whichever is lesser: the advertized
window or the computed congestion window. But is also possible for a
congestion control mechanism to compute the rate at which the network
is able to deliver packets, and to pace transmissions accordingly. The
observed rate is just the number of bytes delivered over some time
period, such as the measured RTT. We will see example mechanims do
exactly that in Chapter 5.

Control-based versus Avoidance-based
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The final implementation choice we draw attention to somewhat subtle.
The challenge is for the the end-host, based on feedback and
observations, to compute how much capacity is available in the
network, and throttle its sending rate accordingly. There are two
general strategies for doing this: an aggresive approach that
purposely causes packet loss and then responds to it, and a
conservative approach that tries to detect the onset of queue build-up
and slows down before they actually overflow.  We refer to the
mechanisms of the first type as *control-based*, and we refer to
mechanisms of the second type as *avoidance-based*.

This distinction is often overlooked—and the term "congestion control"
is used generically to refer to both—but our take is that the
distinction represents and important difference, and so we will call
it out when appropriate.  Admittedly, we will also fall back to the
generic use of "congestion control" when the distinction is not
critical to the discussion, but we will say "control-based" or
"avoidance-based" when the distinction is relevant.

3.2 Evaluation Criteria
-----------------------

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
can be at odds with each other. One way to increase throughput is to
allow as many packets into the network as possible, so as to drive the
utilization of all the links up to 100%. We would do this to avoid the
possibility of a link becoming idle because an idle link necessarily
hurts throughput. The problem with this strategy is that increasing
the number of packets in the network also increases the length of the
queues at each router. *Persistent queues*, in turn, mean packets are
delayed longer in the network, or worse, dropped. Having to drop
packets in the middle of the network not only impacts delay but also
hurts throughput because link bandwidth has been wasted on a packet
that was not successfully delivered to the destination.\ [#]_

.. [#]
	We sometimes use the term *goodput* instead of *throughput* to
	emphasize that we care about data that is successfully delivered
	through the network to the receiver, as opposed to just transmitted
	by the sender.

The ratio of throughput to delay is a general metric for evaluating
the effectiveness of a resource allocation scheme. This ratio is
sometimes referred to as the *power* of the system:

::

   Power = Throughput / Delay

Intuitively, the objective is to maximize this ratio, which is a
function of how much load you place on the system. The load, in turn,
is set by the resource allocation mechanism. :numref:`Figure %s
<fig-power>` gives a representative power curve, where, ideally, the
resource allocation mechanism would operate at the peak of this
curve. To the left of the peak, the mechanism is being too
conservative; that is, it is not allowing enough packets to be sent to
keep the links busy. To the right of the peak, so many packets are
being allowed into the network that either (a) increases in delay due
to queuing are starting to dominate any small gains in throughput,
or (b) throughput actually starts to drop due to packets being
dropped.
   
.. _fig-power:
.. figure:: figures/f06-03-9780123850591.png
   :width: 350px
   :align: center

   Ratio of throughput to delay as a function of load.

Finally, many congestion-control schemes are able to control load in
only very crude ways; that is, it is simply not possible to turn the
“knob” a little and allow only a small number of additional packets
into the network. As a consequence, we need to be concerned about what
happens even when the system is operating under heavy load—towards the
right end of the curve in :numref:`Figure %s <fig-power>`. Ideally, we
would like to avoid the situation in which the system throughput
approaches zero. The goal is for the mechanism to be *stable*—where
packets continue to get through the network even when the network is
operating under heavy load. For a mechanism to not be stable under
certain circumstances is the very definition of *congestion collapse*.

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

.. _reading_jain:
.. admonition:: Further Reading 

	R. Jain, D. Chiu, and W. Hawe. `A Quantitative Measure of Fairness
	and Discrimination for Resource Allocation in Shared Computer Systems
	<https://www.cse.wustl.edu/~jain/papers/ftp/fairness.pdf>`__.
	DEC Research Report TR-301, 1984.

Comparative Analysis
~~~~~~~~~~~~~~~~~~~~~~~

The first step in evaluating any congstion control mechanism is to
measure its performance in isolation, including:

* The average throughput (goodput) flows are able to achieve.

* The avarage end-to-end delay flows experience.

* That the mechanism avoid persistent queues across a range of
  operating scenarios.

* That the mechanism be stability across a range of operating
  scenarios.

* The degree to which flows receive a fair share of the available
  capacity.

The inevitable second step is to compare two or more mechanisms.
Comparing quantitative metrics like throughput is easy. The problem is
how to evaluate multiple mechanism that might coexist, competing with
each other for network resources. Here, the question not whether a
given mechanism treats all of its flows fairly, but whether mechanism
A is fair to flows managed by mechanism B. If mechanism A is is able
to measure improved thoughput over B, but it does so by being more
aggresive, and hence, stealing bandwidth from B's flows, then A's
improvement is not fairly gained and should be discounted.

.. _reading_ware:
.. admonition:: Further Reading

   R. Ware, et. al. `Beyond Jain's Fairness Index: Setting the Bar for
   the Deployment of Congestion Control Algorithms
   <https://www.cs.cmu.edu/~rware/assets/pdf/ware-hotnets19.pdf>`__.
   ACM SICOMM HotNets. November 2019.

Arguments like this have been made many times over the last 30 years,
almost always to the advantage of the incumbent algorithm. But such
analysis suffers from three problems, as identified by Ranysh Ware and
colleagues:

* **Ideal-Driven Goalposting:** A fairness-based threshold asserts
  new mechanism B should equally share the bottelneck link with
  currently deployed mechanism A. This goal is too idealistic in
  practice, especially when A is sometimes unfair to its own flows.

* **Throughput-Centricity:** A fairness-based threshold focuses on
  how new mechanism B impacts a competitor flow using mechanism A
  by focusing on A’s achieved throughput.  However, this ignores other
  important figures of merit for good performance, such as latency,
  flow completion time, or loss rate.

* **Assumption of Balance:** Inter-mechanism interactions often have
  some bias, but a fairness metric cannot tell whether the outcome
  is biased for or against the status quo. It makes a difference in
  terms a deployability whether a new mechanism B takes a larger
  ahare of bandwidth than legacy mechanism A or leaves a larger
  share for A to consume: the former might elicit complaints from
  legacy users of A, where the latter would not. Jain’s Fairness
  Index assigns an equivalent score to both scenarios.

Instead of a simple calculation of Jain's fairness index, Ware
advocates for a threshold based on *harm*, as measured by a reduction
in throughput or an increase in latency. Intuitively, if the amount of
harm caused by flows using a new mechanism B on flows using existing
mechanism A is within a bound derived from how much harm A-managed
flows cause other A-managed flows, we can consider B deployable
alongside A without harm. Ware goes on to propose concrete measures of
acceptable harm, which we revisit for specific pair-wise comparisons
throughout the book.

