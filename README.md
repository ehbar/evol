# evol
Evol: the non-life evolution simulator

Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.

Description
===========

Evol allows organisms (called _lifeforms_) to compete for resources (_energy_)
in order to grow and reproduce.  Each lifeform has a small program which tells
it how to behave; this is called _Dna_ (note the lower-case spelling).  A
lifeform's Dna has one or more opcodes.

Each turn a lifeform exists, it will gather energy from the square it's on, and
any adjacent unoccupied squares.  If the lifeform has enough energy, it will
split, producing a child organism which immediately begins its own competitive
journey.  However, if the lifeform runs out of energy, it will die, its Dna
lost.

Every time a lifeform produces a child, there is a random chance of Dna
mutation.  A random number of opcodes will be added, sliced, removed, or
otherwise mangled.  Many of these changes will be useless; still others will be
detrimental; but some are quite favorable and will give the lifeform and its
descendents a great advantage.

The technical stuff
-------------------

When run, Evol will start one EvolEngine for each core on your computer.  Each
EvolEngine has an Arena, which is a simple cartesian grid that supplies a fixed
amount of energy per square per turn.  These grids will be seeded with a small
number of lifeforms with simple Dna: a single instruction to `MOVE_RANDOM`.

Each turn in every engine, all lifeforms' Dna sequences are executed, and an
action for that lifeform is selected.  Current actions are to move in a
cardinal direction (north, south, east, west), `MOVE_RANDOM`, `APOPTOSIS`
(suicide), or to do nothing (if Dna execution ends without a terminating
instruction).  Some actions (currently only `MOVE_RANDOM`) may impose an extra
energy cost.  Additionally, a small energy cost for each opcode in a lifeform's
Dna is assessed, to encourage efficiency and keep them from getting too long.

From time to time, lifeforms will be moved between the EvolEngines; this is
done by the _Asteroid_, which is a buffer of lifeforms plucked from the grid.
Swapping lifeforms between engines encourages faster evolution and smooths out
the disparity in CPU time some engines might see due to external conditions
like operationg system vagaries and other programs using CPU.  (This design
was chosen to avoid the much more complicated and less efficient locking scheme
that would be required if a single engine ran on all cores.)

Every so often, the Dumper thread will output all lifeforms' Dna to the file
`lifeform-dump.json`.  This is human-readable, but
[reduce_lifeform_opcodes.py](reduce_lifeform_opcodes.py) has been provided for
easier analysis.

There are many tunable settings in [Params.h](Params.h) which you are
encouraged to explore!

OK, so what do I do with it?
----------------------------
The short answer: run this for anything between 5 minutes and overnight.  Send
the resulting JSON dump through the analysis script.  Look for what actions the
lifeforms have evolved to live through thousands of generations!

Copying
=======

Evol is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
either version 3 of the License, or (at your option) any later version.

Evol is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
Evol, in the file [COPYING](COPYING).  If not, see
<http://www.gnu.org/licenses/>.
