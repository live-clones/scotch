Scotch: a software package for graph and mesh/hypergraph partitioning, graph clustering, and sparse matrix ordering
===================================================================================================================

The **Scotch** distribution is a set of programs and libraries which implement the static mapping and sparse matrix reordering algorithms developed within the **Scotch** project.

**Scotch** has many interesting features:

* Its capabilities can be used through a set of stand-alone programs as well as through the **Scotch** library, which offers both C and Fortran interfaces.

* It provides algorithms to partition graph structures, as well as mesh structures defined as node-element bipartite graphs and which can also represent hypergraphs.

* The **Scotch** library dynamically takes advantage of POSIX threads to speed-up its computations. **The PT-Scotch** library, used to manage very large graphs distributed across the nodes of a parallel computer, uses the MPI interface as well as POSIX threads.

* It can map any weighted source graph onto any weighted target graph. The source and target graphs may have any topology, and their vertices and edges may be weighted. Moreover, both source and target graphs may be disconnected. This feature allows for the mapping of programs onto disconnected subparts of a parallel architecture made up of heterogeneous processors and communication links.

* It computes amalgamated block orderings of sparse matrices, for efficient solving using BLAS routines.

* Its running time is linear in the number of edges of the source graph, and logarithmic in the number of vertices of the target graph for mapping computations.

* It can handle indifferently graph and mesh data structures created within C or Fortran programs, with array indices starting from 0 or 1.

* It offers extended support for adaptive graphs and meshes through the handling of disjoint edge arrays.

* It is dynamically parametrizable thanks to strategy strings that are interpreted at run-time.

* It uses system memory efficiently, to process large graphs and meshes without incurring out-of-memory faults.

* It is highly modular and documented. Since it is available under a free/libre software license, it can be used as a testbed for the easy and quick development and testing of new partitioning and ordering methods.

* It can be easily interfaced to other programs. The programs comprising the **Scotch** project have been designed to run in command-line mode without any interactive prompting, so that they can be called easily from other programs by means of system() or popen() calls, or piped together on a single command line. Moreover, vertex labeling capabilities allow for easy renumbering of vertices.

* It provides many tools to build, check, and display graphs, meshes and matrix patterns.

* It is written in C and uses the POSIX interface, which makes it highly portable. **PT-Scotch** uses the **MPI** interface, and optionally the POSIX **Pthreads**.


Obtaining Scotch
----------------

**Scotch** is publicly available under the CeCILL-C free software license, as described [here](https://gitlab.inria.fr/scotch/scotch/blob/master/LICENSE_en.txt). The license itself is available [here](https://gitlab.inria.fr/scotch/scotch/-/blob/master/doc/CeCILL-C_V1-en.txt).

To use lastest version of **Scotch**, please clone the master branch:

    git clone git@gitlab.inria.fr:scotch/scotch.git

Tarballs of the **Scotch** releases are available [here](https://gitlab.inria.fr/scotch/scotch/-/releases).

Documentation
-------------

The most recent version of the user and maintenance manuals are available [here](https://gitlab.inria.fr/scotch/scotch/tree/master/doc).

Installation
------------

Two flavors of installation are available:

* with CMake;

* with a traditional Makefile.

CMake installation is easy and straightforward. It allows one to compile and install **Scotch** and **PT-Scotch**, depending on flags such as the use of POSIX Pthreads and/or MPI. The traditional Makefile installation gives additional freedom to perform (cross-)compilation for non-standard systems and configurations.


Contributing to Scotch
----------------------

To report a bug or discuss potential improvements, You can contact directly the PI at <francois.pellegrini@u-bordeaux.fr>. However, the GitLab environment provides features that are worth taking advantage of, so we recommend you to take the time to use them. Before reporting a bug or submitting a patch in the Inria GitLab environment, you will need an account on the server.
**Please dot not hesitate to send an e-mail to <marc.fuentes@inria.fr> so that we create an account for you on the Inria Gitlab repository**. You will then be able to open issues in the bug tracker, request features, or propose patches using the "merge requests" feature.

Past and current contributors
-----------------------------

The following people contribute(d) to the development of **Scotch**:

* Cédric CHEVALIER

* Sébastien FOURESTIER

* Marc FUENTES

* Jun-Ho HER

* Amaury JACQUES

* Cédric LACHAT

* Selmane LEBDAOUI

* François PELLEGRINI (PI)

* Florent PRUVOST

* Luca SCARANO

Citing Scotch
-------------

Feel free to use the following publications to reference **Scotch**:

* "Scotch and PT-Scotch Graph Partitioning Software: An Overview"
  https://hal.inria.fr/hal-00770422

* "PT-Scotch: A tool for efficient parallel graph ordering"
  https://hal.inria.fr/hal-00402893

Licence
-------

https://gitlab.inria.fr/scotch/scotch/blob/master/LICENSE_en.txt
