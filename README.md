# MCTS

## Description

This repository contains C++17 implementation of the Monte Carlo tree search algorithm (MCTS) using node recycling [4] adapted to transposition tables.
It is accompanied by a [QT](https://www.qt.io/) graphical interface for the board game Omega as a demo.

There is also a custom [generator](https://github.com/Aenteas/cmake-generator) under the scripts folder that provides automatic [CMake](https://cmake.org/) file generation with a support for QT and python wrappers [(SWIG)](http://www.swig.org).

The goal of this project is to provide a versatile C++ implementation of the MCTS algorithm that can be deployed in different board game engines/languages with little effort.

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

Examples are shown on below images with a budget of 8 and transposition table size of 10.

What are the benefits of node recycling?

* Can be customized for memory requirements
* No heap allocation is needed during the search phase
* Can utilize open addressing hash tables
* Hash independent node replacement

Updating the node order when the budget is not exceeded:

![Alt text](add.png?raw=true "adding")

In hash tables a collision occurs when 2 or more nodes are mapped to the same entry. There are 2 common approaches to handle such events, chaining and open addressing. With chaining we have a linked list/vector/array at each entry to store multiple nodes while open addressing stores each elemenent in the hash table itself. Open addressing is preferred as long as the load factor is kept low, which is the main feature of node recycling.

Each time a collision occurs with open addressing, nodes can be placed to the next available slot called linear probing approach. Emptied slots should be marked with a special deleted flag otherwise nodes with a hash value earlier than the emptied cell, but that are stored in a position later than the emptied cell could be reported as not found during lookups. The problem with this approach is that it increases the load factor of the table and slows down the lookup time. 

Since the number of lookups are much higher than the number of deletion during Monte Carlo tree search, it is better to fill up the deleted slots as we insert new nodes.
When a cell i is emptied, it is necessary to search forward through the following cells of the table until finding either another empty cell or a node that can be moved to cell i (that is, a node whose hash value is equal to or earlier than i). When an empty cell is found, then emptying cell i is safe and the deletion process terminates. But, when the search finds a node that can be moved to cell i, it performs this move. This has the effect of speeding up later searches for the moved node, but it also empties out another cell, later in the same block of occupied cells. The search for a movable nodes continues for the new emptied cell, in the same way, until it terminates by reaching a cell that was already empty. In this process of moving nodes to earlier cells, each node is examined only once. Therefore, the time to complete the whole process is proportional to the length of the block of occupied cells containing the deleted node, matching the running time of the other hash table operations [5].

Node recycling and recursive replacement for transposition table updates:

![Alt text](recycling.png?raw=true "recycling")

### Implementation details

* UCT-2 [2] and RAVE [3] exploration strategies.
* Node recycling implementation from [4] adapted to linear probing transposition tables. As a benchmark, there is a simple transposition table implementation with chaining
* No heap allocation during the search phase
* Template-based implementation without virtual calls
* Move-Average Sampling Technique (MAST) simulation policy.
* Parabolic time allocation with early termination (when the best action can not change within the remaining time). The parabolic profile enables uneven time distribution (E.g. giving more budget on middle-game actions)

## TODOS

* Parallel execution and thread interruption
* Add python chess game interface

### Omega game rules

Gameplay:

![](gameplay.gif)

Omega was born as an experiment on complexity and intuitive arithmetic, making a cross between Hex and Go. The game is played by
2 or more players, each trying to create groups of their colors by placing stones in a hexagonal grid, in order to score the most points, but 
each player places stones of all colors in each turn. The scores for each player is calculated by multiplying the sizes of disconnected groups 
of their respective colors.

### References

[1] https://www.redblobgames.com/grids/hexagons/

[2] Childs, B. E., Brodeur, J. H., & Kocsis, L. (2008, December). Transpositions and move groups in Monte Carlo tree search. In 2008 IEEE Symposium On Computational Intelligence and Games (pp. 389-395). IEEE.

[3] Gelly, S., & Silver, D. (2011). Monte-Carlo tree search and rapid action value estimation in computer Go. Artificial Intelligence, 175(11), 1856-1875.

[4] Powley, E., Cowling, P., & Whitehouse, D. (2017, September). Memory bounded monte carlo tree search. In Proceedings of the AAAI Conference on Artificial Intelligence and Interactive Digital Entertainment (Vol. 13, No. 1).

[5] https://en.wikipedia.org/wiki/Linear_probing