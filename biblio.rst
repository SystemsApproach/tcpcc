Annotated Bibliography
=======================

The set of research papers published on congestion control is
extensive, with only a small subset cited in the main body of the
book. This section is a place to collect a more comprehensive
bibliography, which (for now) is organized according the major themes
covered in the book.

We invite the community help keep the bibliography complete and
up-to-date. Please submit a `Pull Request to GitHub
<https://github.com/SystemsApproach/tcpcc>`__ to include additional
citations or to fix mistakes. Post an `Issue to GitHub
<https://github.com/SystemsApproach/tcpcc/issues>`__ if you have
suggestions for ways to improve how the bibliography is organized.

Foundational
-----------------

Queuing Analysis
~~~~~~~~~~~~~~~~~~~~

* L. Kleinrock. `Queueing Systems, Volume 2
  <https://archive.org/details/queueingsystems02klei>`__.  Wiley &
  Sons, May 1976.

* V. Paxson and S. Floyd. `Wide-Area Traffic: The Failure of Poisson
  Modeling <https://www.icir.org/vern/papers/poisson.TON.pdf>`__.
  IEEE/ACM Transactions on Networking, June 1995.
         
* W. Leland *et al*, `On the self-similar nature of Ethernet traffic
  <https://doi.org/10.1145/167954.166255>`__.
  ACM SIGCOMM '93 Symposium, August 1993.

* J. Gettys. `Bufferbloat: Dark Buffers in the Internet 
  <https://ieeexplore.ieee.org/document/5755608>`__.
  IEEE Internet Computing, April 2011.  

Theoretical Underpinnings
~~~~~~~~~~~~~~~~~~~~~~~~~~~

* M. Mathis, J. Semke, J. Mahdavi, and T. Ott. `The Macroscopic
  Behavior of the TCP Congestion Avoidance Algorithm
  <https://dl.acm.org/doi/abs/10.1145/263932.264023>`__.
  SIGCOMM CCR, 27(3), July 1997.

* F. Kelly. `Charging and Rate Control for Elastic Traffic
  <http://www.statslab.cam.ac.uk/~frank/elastic.pdf>`__.
  European Transactions on Telecommunications, 8:33–37, 1997.

* S. Athuraliya and S. Low. `An Empirical Validation of a Duality
  Model of TCP and Active Queue Management Algorithms
  <https://ieeexplore.ieee.org/document/977445>`__.
  Proceedings of the Winter Simulation Conference, 2001.

* R. Jain and K. K. Ramakrishnan. `Congestion Avoidance in Computer
  Networks with a Connectionless Network Layer: Concepts, Goals and
  Methodology. <https://arxiv.org/pdf/cs/9809095.pdf>`__.  Computer
  Networking Symposium, April 1988.

Evaluation Criteria
~~~~~~~~~~~~~~~~~~~~
  
* R. Jain, D. Chiu, and W. Hawe. `A Quantitative Measure of Fairness
  and Discrimination for Resource Allocation in Shared Computer Systems
  <https://www.cse.wustl.edu/~jain/papers/ftp/fairness.pdf>`__.
  DEC Research Report TR-301, 1984.

* Bob Briscoe. `Flow Rate Fairness: Dismantling a Religion 
  <https://web.stanford.edu/class/cs244/papers/fair-ccr2007.pdf>`__. 
  ACM SIGCOMM CCR, April 2007. 

* R. Ware, *et al*. `Beyond Jain's Fairness Index: Setting the Bar for
  the Deployment of Congestion Control Algorithms
  <https://www.cs.cmu.edu/~rware/assets/pdf/ware-hotnets19.pdf>`__.
  ACM SIGCOMM HotNets. November 2019.

Architecture
~~~~~~~~~~~~~

* J. Saltzer, D. Reed, and D. Clark. `End-to-End Arguments in System Design 
  <https://web.mit.edu/Saltzer/www/publications/endtoend/endtoend.pdf>`__. 
  ACM Transactions on Computer Systems, Nov. 1984. 

* D. Clark, `The Design Philosophy of the DARPA Internet Protocols 
  <https://dl.acm.org/doi/10.1145/52324.52336>`__. 
  ACM SIGCOMM, 1988.

* S. Jain, *et al*. `B4: Experience with a 
  Globally-Deployed Software Defined WAN 
  <https://cseweb.ucsd.edu/~vahdat/papers/b4-sigcomm13.pdf>`__.
  ACM SIGCOMM, August 2013.

* J. Perry, *et al*. `Fastpass: A Centralized "Zero-Queue" Datacenter Network 
  <http://fastpass.mit.edu/Fastpass-SIGCOMM14-Perry.pdf>`__.
  ACM SIGCOMM, August 2014.


General-Purpose Algorithms
--------------------------------

* V. Jacobson. `Congestion Avoidance and Control
  <https://dl.acm.org/doi/10.1145/52324.52356>`__.  ACM SIGCOMM '88
  Symposium, August 1988.

* Janey Hoe. `Improving the start-up behavior of a congestion control
  scheme for TCP
  <https://dl.acm.org/doi/10.1145/248156.248180>`__.  SIGCOMM '96.
  August 1996.

* L. Brakmo, S. O'Malley and L. Peterson
  `TCP Vegas: New Technique for Congestion Detection and Avoidance
  <https://sites.cs.ucsb.edu/~almeroth/classes/F05.276/papers/vegas.pdf>`__.
  ACM SIGCOMM '94 Symposium. August 1994. (Reprinted in *IEEE/ACM Transactions 
  on Networking,* October 1995).

* S. Low, L. Peterson, and L. Wang. `Understanding TCP Vegas: A
  Duality Model. <https://dl.acm.org/doi/10.1145/506147.506152>`__.
  Journal of the ACM, Volume 49, Issue 2, March 2002.

* N. Cardwell, Y. Cheng, C. S. Gunn, S. Yeganeh, V. Jacobson.
  `BBR: Congestion-based Congestion Control
  <https://cacm.acm.org/magazines/2017/2/212428-bbr-congestion-based-congestion-control/fulltext>`__. 
  Communications of the ACM, Volume 60, Issue 2, February 2017.  

Active Queue Management
---------------------------------

* K.K. Ramakrishnan and R. Jain. `A Binary Feedback Scheme for
  Congestion Avoidance in Computer Networks with a Connectionless
  Network Layer <https://dl.acm.org/doi/pdf/10.1145/52324.52355>`__.
  ACM SIGCOMM, August 1988.

* S. Floyd and V.  Jacobson `Random Early Detection (RED)  Gateways for Congestion Avoidance
  <http://www.icir.org/floyd/papers/early.twocolumn.pdf>`__.
  IEEE/ACM Transactions on Networking. August 1993.

* R. Braden, *et al*. `Recommendations on Queue Management and
  Congestion Avoidance in the Internet
  <https://tools.ietf.org/html/rfc2309>`__. RFC 2309, April 1998.

* K. Ramakrishnan, S. Floyd, and D. Black.  `The Addition of Explicit
  Congestion Notification (ECN) to IP
  <https://datatracker.ietf.org/doc/html/rfc3168>`__.  RFC 3168,
  September 2001.

* K. Nichols and V. Jacobson. `Controlling Queue Delay 
  <https://queue.acm.org/detail.cfm?id=2209336>`__. 
  ACM Queue, 10(5), May 2012. 

Domain-Specific Algorithms
-------------------------------

Datacenter
~~~~~~~~~~~~~~~~

* M. Alizadeh, *et al*. `Data Center TCP (DCTCP)
  <http://dl.acm.org/citation.cfm?doid=1851182.1851192>`__.  
  ACM SIGCOMM, August 2010.
   
* Radhika Mittal, et al. `TIMELY: RTT-based Congestion Control for the Datacenter
  <https://conferences.sigcomm.org/sigcomm/2015/pdf/papers/p537.pdf>`__.
  ACM SIGCOMM 2015.

* S. Liu, *et al*. `Breaking the Transience-Equilibrium Nexus: A New
  Approach to Datacenter Packet Transport
  <https://www.usenix.org/system/files/nsdi21-liu.pdf>`__.
  Usenix NSDI '21. April 2021.

Background Transfers
~~~~~~~~~~~~~~~~~~~~~~~

* S. Shalunov, *et al*. `Low Extra Delay Background Transport (LEDBAT)
  <https://www.rfc-editor.org/info/rfc6817>`__.  
  RFC 6817, December 2012.

HTTP   
~~~~~~~~~~~~

* J. Iyengar and I. Swett, Eds.
  `QUIC Loss Detection and Congestion Control
  <https://www.rfc-editor.org/info/rfc9002>`__.  
  RFC 9002, May 2021.

Wireless
~~~~~~~~~~~~~~

* H. Jiang, Z. Liu, Y. Wang, K. Lee and I. Rhee. 
  `Understanding Bufferbloat in Cellular Networks 
  <https://conferences.sigcomm.org/sigcomm/2012/paper/cellnet/p1.pdf>`__
  ACM SIGCOMM Workshop on Cellular Networks, August 2012.

* Y. Xie, F. Yi, and K. Jamieson. `PBE-CC: Congestion Control via
  Endpoint-Centric, Physical-Layer Bandwidth Measurements
  <https://arxiv.org/abs/2002.03475>`__. SIGCOMM 2020.  

* Yihua Gao, et al. `Understanding On-device Bufferbloat For Cellular
  Upload. <https://www-users.cse.umn.edu/~fengqian/paper/bufferbloat_imc16.pdf>`__.


Realtime
~~~~~~~~~~~~~~~

* S. Floyd, M. Handley, J. Padhye, and J. Widmer.
  `TCP Friendly Rate Control (TFRC): Protocol Specification
  <https://www.rfc-editor.org/info/rfc5348>`__.  
  RFC 5348, September 2008.

* J. Padhye, V. Firoiu, D. Towsley, and J. Kurose.
  `Modeling TCP Throughput: A Simple Model and its Empirical Validation
  <https://conferences.sigcomm.org/sigcomm/1998/tp/paper25.pdf>`__.
  ACM SIGCOMM, September 1998.

Implementations and Tools
--------------------------------

* S.J. Leffler, M.K. McKusick, M.J. Karels, and J.S Quarterman. `The
  Design and Implementation of the 4.3 BSD Unix Operating  <https://www.goodreads.com/en/book/show/5770.The_Design_and_Implementation_of_the_4_3BSD_UNIX_Operating_System>`__. Addison-Wesley. January 1989.

* `Netesto <https://github.com/facebook/fbkutils/tree/master/netesto>`__.

* `NS-3 Network Simulator <https://www.nsnam.org>`__.

* `RFC 6298: Computing TCP's Retransmission Timer
  <https://tools.ietf.org/html/rfc6298>`__. June 2011.

* The Linux Kernel. `BPF Documentation
  <https://www.kernel.org/doc/html/latest/bpf/index.html>`__.
