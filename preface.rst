Preface
=======

Congestion control has been one of the most active areas of research
in computer networking from the earliest days of packet switching. The
work of Jacobson and Karels in the 1980s laid the foundation for
decades of subsequent work by introducing a suite of congestion
control mechanisms into TCP. This was done at a time of crisis, with
the Internet showing signs of congestion collapse.  Ethernet inventor
Bob Metcalf famously predicted the Internet would collapse in the
1990s and followed up on his promise to eat his words when it did
not. But it was clear even then that congestion control was not a
fully solved problem, and improvements to the algorithms on which the
Internet's smooth functioning depends have multiplied ever since.

This book grew out of our own involvement in developing congestion
control algorithms over the last three decades. There have been so
many developments in congestion control over that time that it’s
nearly impossible to include all of them. What we have tried to do in
this book is provide a framework for understanding congestion control
as a systems problem, and to characterize the many approaches along a
few main themes. For example, our work on TCP Vegas opened up a line
of research that continues today, where the aim is to avoid severe
congestion rather than react after it has set in. We thus consider
avoidance-based approaches as one of the main categories of congestion
control.

We expect this to be an evolving manuscript. There are many efforts in
congestion control that are not currently covered, the algorithms that
are covered continue to be refined, and new approaches will likely
emerge to address new use cases.  We will update the book as necessary
to reflect the state of the field. Please help by submitting your
comments and feedback. We also welcome contributions to the on-line
annotated bibliography.

Finally, we extend our thanks to those who have contributed to the
open source effort to improve this book. They include:

- Bill Fisher
- Giulio Micheloni
- J van Bemmel
- Omer Shapira
- Nico Vibert
- Vik Vanderlinden
- Vidhi Goel

Please send
us your comments and feedback using the `Issues Link
<https://github.com/SystemsApproach/tcpcc/issues>`__. See the `Wiki
<https://github.com/SystemsApproach/tcpcc/wiki>`__ for the latest todo
list.

| Larry Peterson, Lawrence Brakmo, and Bruce Davie
| May 2022

