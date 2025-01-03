                 |---------------------.
                 | David L. F. Gaden's |
.----------------|---------------------'
|  plcEmulator   |  Version:    0.2.0
'----------------|
                 |  If you use, please cite:
                 V
* D. L. F. Gaden, and E. L. Bibeau (2012), "A programmable logic controller
  emulator with automatic algorithm-switching for computational fluid dynamics
  using OpenFOAM," Proceedings of the 2012 CSME International Congress, Paper
  number 156959763, June 4-6, Winnipeg, MB.

Description
===========
Programmable logic controller emulator for OpenFOAM.  With a plcEmulator-
enabled solver, a user can define outputs, inputs and control logic for their
simulation.  Also allows the simulation to switch between algorithms depending
on conditions.

Original Author
===============
David L. F. Gaden (dlfgaden@gmail.com)

Current Maintainer
==================
David L. F. Gaden (dlfgaden@gmail.com)

Contributors
============
David L. F. Gaden : base version

Documentation
=============
Coming soon.

Installation/Compilation
========================
Refer to the documentation.

Contents
========
 - src - the library source files
 - applications - a demo application that uses plcEmulator
 - tutorials - a sample case that runs the demo application


Required OpenFOAM-Version (Known to work with)
==============================================
1.6-ext
2.1.x

plcEmulator also requires equationReader and multiSolver extensions.


History
=======
 2012-10-25: Initial import
