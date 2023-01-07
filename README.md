# MCTS

## Description

This repository contains a fully customizable Monte Carlo tree search algorithm (MCTS) in C++17. The implementation uses policy based template design (no virtual calls) to make the following behaviours configurable:

* Exploration strategy - Determines selection and backpropagation phases during the search
* Transposition table - Provides memory management and storage for the tree nodes
* Rollout policy - Specifies the strategy used for the simulation phase
* Scheduler - Time allocation strategy for each search phase
* Game type - The type of board game to play

Other configurable parameters:
* The maximum number of tree nodes to allocate during the search.
* Number of parallel threads (TODO)

Type of supported board games:
2 players with arbitrary number of pieces where players can have sub-actions (like in omega where each player places 2 pieces in each turn)

The goal of this project is to provide C++ implementation that can be extended with different MCTS variations and deployed in different board game engines with little effort.

## Features

* Exploration strategy - Upper confidence tree (UCT-2) [2]. Rapid Action Value Estimation (RAVE) [3].
* Transposition table - Node recycling implementation from [4] adapted to linear probing transposition tables. As a benchmark, there is a standard transposition table implementation with chaining
* Rollout policy - Move-Average Sampling Technique (MAST) and random simulation policies.
* Scheduler - Parabolic time allocation with early termination (when the best action can not change within the remaining time). The parabolic profile enables uneven time distribution (E.g. giving more budget on middle-game actions)
* Game type - Omega accompanied by a [QT](https://www.qt.io/) graphical interface.
* No virtual call or heap allocation during the search.
* Search can be interrupted and continued

There is also a custom [generator](https://github.com/Aenteas/cmake-generator) under the scripts folder that provides automatic [CMake](https://cmake.org/) file generation with a support for QT and python wrappers [(SWIG)](http://www.swig.org).

## TODOS

* Working on tree parallelisation: it is definitely not an easy one:)
* Add python wrapper and demo

## Requirements

* CMake v3.22.1+
* A c++17 compiler (defaults to gcc)
* make
* python3.6+ interpreter
* SWIG
* qt5

## Setup

at root directory:
```
$ ./generate.sh
$ ./configure.sh
$ ./build.sh
```

install:
```
$ ./install.sh
```

run executable:
```
$ ./run-exe.sh
```

for more details to set up python interface visit this [repository](https://github.com/Aenteas/cmake-generator).

### Node recycling with transposition tables

What is node recycling? The idea is to discard the least recently visited leaf nodes from the search tree as the number of nodes exceeds a budget. Nodes are stored in a fixed-size (budget) FIFO-like container to determine their relative importance order, with the next node to be discarded at the front. 

During the selection phase we push the selected nodes to the back of the container where we assign higher importance for the nodes closer to the root. In the expansion step we overwrite the node at the front and update its position in the container. When the number of nodes exceeds the budget, the node to be overwritten is the least recently visited leaf node or an unreachable node (node recycling).

By combining the work from [4] with transposition tables, the next node to be replaced won't necessarily be a leaf node. This is because nodes can have multiple parents due to transpositions. Nevertheless, the recycled node can not be any from the current selection path when using a single thread and the approach still remains a sensible node replacement strategy.

Examples are shown on below images with a budget of 8 and transposition table size of 10.

What are the benefits of node recycling?

* Can be customized for memory requirements
* No heap allocation is needed during the search phase
* Hash independent node replacement
* A good fit for tree parallelization because as opposed to standard node replacement strategies, nodes are not being replaced randomly (hash dependence) giving us more control to prevent nodes from being overridden before the results are backpropagated to them.

Updating the node order when the budget is not exceeded:

![Alt text](add.png?raw=true "adding")

In hash tables a collision occurs when 2 or more nodes are mapped to the same entry. There are 2 common approaches to handle such events, chaining and open addressing. With chaining we have a linked list/vector/array at each entry to store multiple nodes while open addressing stores each elemenent in the hash table itself. Open addressing is preferred as long as the load factor is kept low, which is the main feature of node recycling.

Each time a collision occurs with open addressing, nodes can be placed to the next available slot called linear probing approach. Emptied slots should be marked with a special deleted flag otherwise nodes with a hash value earlier than the emptied cell, but that are stored in a position later than the emptied cell could be reported as not found during lookups. The problem with this approach is that it increases the load factor of the table over time and slows down the lookup time. 

Since the number of lookups are much higher than the number of deletion during Monte Carlo tree search, it is better to fill up the deleted slots as we insert new nodes.
When a cell i is emptied, it is necessary to search forward through the following cells of the table until finding either another empty cell or a node that can be moved to cell i (that is, a node whose hash value is equal to or earlier than i, see node 5 on the figure below as an example). When an empty cell is found, then emptying cell i is safe and the deletion process terminates. But, when the search finds a node that can be moved to cell i, it performs this move. This has the effect of speeding up later searches for the moved node, but it also empties out another cell, later in the same block of occupied cells. The search for a movable nodes continues for the new emptied cell, in the same way, until it terminates by reaching a cell that was already empty. In this process of moving nodes to earlier cells, each node is examined only once. Therefore, the time to complete the whole process is proportional to the length of the block of occupied cells containing the deleted node, matching the running time of the other hash table operations [5].

Node recycling and recursive replacement for transposition table updates:

![Alt text](recycling.png?raw=true "recycling")

### Node recycling with tree parallelisation

One needs to make sure the following to have a correct implementation:

* There is no concurrent backpropagation on the same node. This is an easy one, nodes should be just protected by a simple mutex during selection and backpropagation.

* Both the hash table and the FIFO should have data protection.  
The FIFO container is a doubly-linked list where the pointers are being modified concurrently during selection (when nodes are moved in front of their parents). Therefore the pointers should be provided by data protection.  
Hash table is only modified during expansion. Therefore we can provide exclusive access for expansion and shared access during selection and backpropagation. This can be acheived by shared mutexes. It turns out that we need to lock multiple mutexes at the same time during selection to prevent data corruption:  
When we select the current best child node we must hold a lock on it either until we find the new best child or (if it was the best child) until the best grandchild is found. This is because if we release the lock on the currently best child, an other thread might override it during expansion. As a result, selection would proceed from the new leaf node causing incorrect updates. From this we can conclude that we need to hold 3 mutexes at the same time (one for the parent node, one for the currently best child and an other for the currently visited child). This means that shared access is not only needed for optimization but to ensure correctness without a deadlock (simultaneous locking does not help here). But there is a caveat at this point: we need to make sure that parent-child and sibling states are actually different nodes in the tree (it is not necessarily the case when using transposition). Otherwise we potentially lock the same mutex multiple times from the same thread causing an undefined behavior. Assuming that we are using zobrist hashing (XOR) we can do the following: make sure that there is no action hashkey has only zeros (parent-child case) and all the action hashkeys are unique (sibling case). Alternatively, we could extend shared locking with recursive property (only shared accesses have to be secured this way in our case). This would entail some performance hit so the first approach is preferred.  
We might want to divide the hash table into blocks of arrays each having a separate mutex to reduce the memory footprint. Unfortunately, we would need recursive shared access as multiple nodes can be in the same block.  
It turns out that we can just use a single shared mutex to protect both the node and its corresponding location in the hash table (we also need mutexes in the hash table where there is no node stored).  
* None of the currently selected nodes are recycled. If this is not ensured parent nodes might be invalid during backpropagation, leading to incorrect updates or even crashing and undefined behaviours.  
Unfortunately, this does not hold true when using hash tables not even with node recycling. However, it is a very unlikely event and it only causes incorrect updates:  
What are the odds for such an event? Let's say we have a FIFO of size 1000, 16 threads and one of the threads have 8 nodes selected so far. There will be approximately 1000 - 8 = 992 nodes in front of any of these selected nodes. To have any of these selected nodes recycled, the other ~992 nodes should be moved behind the selectes ones in the FIFO (by selection or expansion) before our thread would backpropagate the results to them. Assuming that leaf nodes are on average 10 nodes from the root (branching factor is around 2) and on average every second visited node is different, the other threads should be on average ~13 times faster (992 / 10 / (16 - 1) * 2) than our thread to have this event happen. In reality, there almost always be a lot of unreachable nodes, each requiring a full MCTS cycle to be moved in the FIFO which would further decrease the likelihood.  
When a selected node is recycled, we would only update the newly created node with the wrong statistics (minor performance hit).  
It is important to store the pointers to each of the selected nodes so the result can be backpropagated to the non-recycled nodes.  
Unlike with node recycling, by using hash dependent replacement approaches a selected node is replaced with a newly created one when their hascodes matches which is a much more likely scenario (can also happen when using a single thread).

### Omega

Gameplay:
Here is a demonstration on how the computer plays as black with the default configuration:

![](gameplay.gif)

Omega was born as an experiment on complexity and intuitive arithmetic, making a cross between Hex and Go. The game is played by
2 or more players, each trying to create groups of their colors by placing stones in a hexagonal grid, in order to score the most points, but 
each player places stones of all colors in each turn. The scores for each player is calculated by multiplying the sizes of disconnected groups 
of their respective colors.

3 game modes are available: Player vs player, player vs computer and computer vs computer.

### References

[1] https://www.redblobgames.com/grids/hexagons/

[2] Childs, B. E., Brodeur, J. H., & Kocsis, L. (2008, December). Transpositions and move groups in Monte Carlo tree search. In 2008 IEEE Symposium On Computational Intelligence and Games (pp. 389-395). IEEE.

[3] https://www.cs.utexas.edu/~pstone/Courses/394Rspring13/resources/mcrave.pdf

[4] Powley, E., Cowling, P., & Whitehouse, D. (2017, September). Memory bounded monte carlo tree search. In Proceedings of the AAAI Conference on Artificial Intelligence and Interactive Digital Entertainment (Vol. 13, No. 1).

[5] https://en.wikipedia.org/wiki/Linear_probing