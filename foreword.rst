Foreword
==========

Congestion control is unquestionably one of the most important, most
fundamental topics in computer networking.  It’s also one of most
challenging, as it requires controlling endpoints that are potentially
distributed around the globe, in different organizations, and
supporting different applications.  The role of the network layer in
supporting transport-layer congestion control is also a multi-faceted,
nuanced challenge. And congestion control is needed in just about
every Internet scenario one can imagine: from the public Internet that
spans the globe and carries all types of traffic, to long “fat” pipes
carrying massive amounts of file-transfer data, to specialized
datacenter networks, to private commercial backbone networks, to
mobile and wireless networks.

With all of these challenges, how does one make sense of the many
(many!) approaches towards congestion control that have been
developed?  What are the fundamental challenges these approaches are
solving?  What is the role of the network layer, and more broadly what
is the design space for congestion control protocols? Are there broad
classes or approaches towards congestion control that can be
identified?  Which approaches have been adopted in practice, and why?
And among those many “flavors”/variations of TCP that you might have
heard about—how do they differ and in what scenarios are they best
used, and why? So many questions!

To make sense of this and to answer all of these questions (and more)
would require not just a book, but a great book!  And now fortunately,
there is such a book—this book! The three authors of *TCP Congestion
Control: A Systems Approach* are among the most knowledgeable
congestion control researchers on the planet—Brakmo and Peterson’s
TCP Vegas protocol (you can learn more about that in section 5.1)
pioneered the notion that endpoints could anticipate and avoid
congestion, rather than react to observed congestion; TCP Vegas has
served as a foundation on which more recent congestion avoidance
protocols (such as the BBR protocol championed by Google, see Section
5.3) have been designed.  The authors are also absolutely fabulous
writers (and I say this as a textbook author myself)—lucid, clear, and
engaging, and able to organize and communicate complex ideas, with
just the right amount of detail and discussion of practice.  The
“systems approach” that Larry Peterson and Bruce Davie have championed
is also exactly what is needed to truly understand congestion control,
where deep, system-wide issues in network architecture come to the
fore (e.g., the separation and interaction of network and transport
layer functionalities; the question of implementing networks services,
such as congestion control, in the either the application layer or in
the network).

This book is a needed and most welcome addition to the fabulous set of
open-source, “systems approach” books that Larry, Bruce and others
have been developing.  I hope you read it cover-to-cover, consult it
again later as you need it in the future, and enjoy it as much as I
have.

| Jim Kurose
| Amherst, Massachusetts
