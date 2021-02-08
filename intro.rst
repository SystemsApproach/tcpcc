Chapter 1:  Introduction
========================

	
The Internet is considered an engineering success with few peers, and
rightfully so. It has scaled to connect billions of devices, supports
every imagined communications application, and accommodates
transmission rates ranging from tens of bits-per-day to hundreds of
gigabits-per-second. But at its core is a thorny technical challenge
that has drawn widespread attention for the last 30-plus years, from both
practitioners trying to make the Internet perform better and
theoreticians wanting to understand its mathematical underpinnings:
how the Internet’s resources are best allocated to all the competing
interests trying to use it.

Resource allocation is a hard problem in any computer system, but
especially so for a system as complex as the Internet. The problem was
not top-of-mind when the Internet’s TCP/IP protocol stack was first
deployed in the early 1980s.  By the end of the decade, however, with the
Internet gaining serious use in universities (but predating
the World Wide Web's invention by several years), the network began
to experience a
phenomenon known as *congestion collapse* [#]_. A solution—congestion
control—was developed and deployed in the late 1980s and the
immediate crisis was addressed. The
Internet community has been studying and refining its approach to
congestion control ever since. This book is about that journey.

.. [#] Ten years earlier a similar effect, called *thrashing*, had plagued the pioneers in time-shared computing systems.
   
The first efforts to manage congestion were undertaken by two
researchers, Jacobson and Karels. The resulting paper, 
*Congestion Avoidance and Control*, published in 1988, is one of the
most cited papers in networking of all time. There are
good reasons for that. One is that congestion collapse really did
threaten the nascent Internet, and the work undertaken to
address it was foundational to the Internet's ultimate
success. Without that work it's unlikely we'd have the global Internet
we have today.

Another reason for the citation impact of this work is that congestion
control has been an amazingly fruitful area of research for over three
decades. Congestion control, and resource allocation more broadly, are
wide open design spaces with plenty of room for innovation. Decades of
research and implementation have built on the early foundations, and it seems fair to assume that new approaches
or refinements to the existing approaches will continue to appear for
as long as the Internet exists.

In this book, we explore the design space for congestion control in
the Internet and present a description of the major approaches to
managing or avoiding congestion that
have been developed over the last three decades. 


.. _reading_vj:
.. admonition:: Further Reading

   V.Jacobson `Congestion Avoidance and Control'
   <https://dl.acm.org/doi/10.1145/52324.52356>`__.
   SIGCOMM '88 Symposium, August 1988.


1.1 What is Congestion?
------------------------

Anyone who has driven on a highway at rush hour has experienced
congestion. There is a limited resource—the space on the highway—and a
set of cars, trucks, etc. that compete for that resource. As rush hour
gets underway, more traffic arrives but the road keeps working as
intended, just with more vehicles on it. But there
comes a point where the number of vehicles becomes so large that
everyone has to slow down (because there is no longer enough space for
everyone to keep a safe distance at the speed limit) at which point the
road actually becomes *less effective* at moving vehicles. So, just at
the point when you would be wanting more capacity, there is actually
less capacity to move traffic. This is the essence of *congestion
collapse*, when congestion is so bad that the systems start to perform
significantly worse than it did without congestion. The mechanism of congestion collapse is quite a bit different for
packet networks than for highways, but it is equally problematic [#]_.

.. [#] Networking people like making analogies between real-world
       congestion and network congestion, but it's important to
       recognize that analogies are imperfect.
   
This book focuses on congestion control for packet-switched networks. A
fundamental aspect in packet switching is *multiplexing*, which means
that a system resource—such as a link or a queue in a router—
is shared among multiple users or applications. Furthermore, packet networks are *statistically
multiplexed*, which means that they are based on the idea that packets
show up somewhat randomly, and we rely on the statistical properties
of those arrivals to ensure that we don't run out of resources. The
existence of congestion collapse shows that sometimes the statistics
don't quite work out as we'd like.

To see how this might work, consider the simple network illustrated in
:numref:`Figure %s <fig-mux>`, where the three hosts on the left side
of the network (senders S1-S3) are sending data to the three hosts on
the right (receivers R1-R3) by sharing a switched network that
contains only one physical link. (For simplicity, assume that host S1
is sending data to host R1, and so on.) In this situation, three flows
of data—corresponding to the three pairs of hosts—are multiplexed onto
a single physical link by switch 1 and then *demultiplexed* back into
separate flows by switch 2. Note that we are being intentionally vague
about exactly what a “flow of data” corresponds to. For the purposes
of this discussion, assume that each host on the left has a large
supply of data that it wants to send to its counterpart on the right.

.. _fig-mux:
.. figure:: figures/f01-05-9780123850591.png
   :width: 500px
   :align: center
   
   Multiplexing multiple logical flows over a single
   physical link.


Statistical multiplexing means that all the hosts in this network send
packets whenever it suits them, and if it happens that several packets
turn up at the same time at a switch, one of them will be transmitted
first while the others are placed in to a queue. So both the link and
the queue are shared resources, and both are finite. The link can
only carry so many bits per second, and the queue can only hold so
many packets (or bytes) before it has to start discarding
packets. Managing the access to these shared resources, and trying to
do so in a way that prevents congestion collapse, is the essence
of congestion control. A switch that occasional puts packets in a
queue is operating normally. A switch that has large numbers of
packets in its queues all or most
of the time is congested. We'll get to the definition of congestion
collapse for networks later on, but it starts with congested switches,
routers or links. 

For a deeper introduction to statistical multiplexing, and why it's
the approach of choice for packet networks, we refer to the
following text.

.. _reading_statmux:
.. admonition:: Further Reading

      `Requirements
      <https://book.systemsapproach.org/foundation/requirements.html>`__.
      *Computer Networks: A Systems Approach*, 2020.


When a switch builds a queue of packets awaiting transmission, it needs to decide which packet gets sent
next. 
Each switch in a packet-switched network makes this decision
independently, on a packet-by-packet basis. One of the issues that arises is how to make this decision in a fair manner. For
example, many switches are designed to service packets on a first-in,
first-out (FIFO) basis. Another approach would be to transmit the
packets from each of the different flows that are currently sending data
through the switch in a round-robin manner. This might be done to ensure
that certain flows receive a particular share of the link’s bandwidth or
that they never have their packets delayed in the switch for more than a
certain length of time. A network that attempts to allocate bandwidth to
particular flows is sometimes said to support *quality of service*
(QoS).

One thing to take away from this discussion is that it is in the
nature of packet-switched networks that they will sometimes be
congested. The focus of this book is on the large body of work that
has been done to mitigate congestion, either by responding to it in
effective ways to lessen it, or be preventing it before it occurs.



1.2 Controlling Congestion
---------------------------

  

Resource allocation and congestion control are complex issues that have
been the subject of much study ever since the first network was
designed. They are still active areas of research. One factor that makes
these issues complex is that they are not isolated to a single level
of a protocol hierarchy. Resource allocation is partially implemented in
the routers, switches, and links inside the network and partially in the
transport protocol running on the end hosts. End systems may use
signalling protocols to convey their resource requirements to network
nodes, which respond with information about resource
availability. Application protocols may themselves be designed to mitigate
congestion, e.g. by changing the resolution of video transmission
based on the current network conditions. This is a canonical example
of a *systems issue*: you can't fully understand congestion without
looking at all the places in the system that it touches.

We should clarify our terminology before going any further. By *resource
allocation*, we mean the process by which network elements try to meet
the competing demands that applications have for network
resources—primarily link bandwidth and buffer space in routers or
switches. Of course, it will often not be possible to meet all the
demands, meaning that some users or applications may receive fewer
network resources than they want. Part of the resource allocation
problem is deciding when to say no and to whom.

We use the term *congestion control* to describe the efforts made by
network nodes (including end systems) to prevent or respond to overload conditions. Since
congestion is generally bad for everyone, the first order of business is
making congestion subside, or preventing it in the first place. This
might be achieved simply by persuading a few hosts to stop sending, thus
improving the situation for everyone else. However, it is more common
for congestion-control mechanisms to have some aspect of fairness—that
is, they try to share the pain among all users, rather than causing
great pain to a few. Thus, we see that many congestion-control
mechanisms have some sort of resource allocation built into them.

It is also important to understand the difference between flow control
and congestion control. Flow control involves keeping a fast sender from
overrunning a slow receiver. Congestion control, by contrast, is
intended to keep a set of senders from sending too much data *into the
network* because of lack of resources at some point. These two concepts
are often confused; as we will see, they also share some mechanisms.

Given all the different places and layers where congestion control and resource
allocation can be implemented, it is helpful to start with a simple
approach, which is pretty much what Jacobson and Karels did (although
their solution ended up having quite a few moving parts).

In the early Internet, routers implemented the most basic resource
allocation approach possible: FIFO queueing with tail drop. There was
no awareness of flows or applications, so they simply accepted packets
as they arrived, placed them in a queue whenever the outbound link
capacity was less than the arrival rate, served the queue by the FIFO
discipline, and dropped arriving packets if they queue was full
("tail-drop"). This is still common today, although other approaches
are also common now.

The reason that congestion collapse occurred in this


1.3 Theoretical Underpinnings
------------------------------
Introduce theoretical underpinnings. Since we are about “systems
approach” it’s ok to stay on the practical side, but it would be
helpful to give the intuition behind the theory (and how it has
evolved since the VJ era). Notably, talk about dynamics/stability and
fairness. 


1.4 Congestion Control Today
----------------------------

identify broader factors that impact the approach: increased
bandwidth, wireless links, new applications, new use domains (i.e.,
previous point). The point is that the problem is not fixed; the
target keeps moving.

1.5 Visualizing Congestion Control
-----------------------------------

Preview the graphical data presented later in the book to illustrate the dynamic behavior of various algorithms. Related to the evaluation criteria from the previous paragraph. This data serves two purposes: (a) helps reader visualize the algorithms dynamic behavior, and (b) helps us evaluate and compare different algorithms.
