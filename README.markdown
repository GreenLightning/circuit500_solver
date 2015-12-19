circuit500_solver
=================

This is a solver for [circuit500](https://itunes.apple.com/us/app/circuit500/id1029430896) levels.

Status
======

This project is still **Work In Progress**. Currently, the solver is able to solve 401 out of the 500 levels (= 80.2 %!) in about 4 minutes (less than half a second per level on average).

Design
======

A tap on a tile can have three possible outcomes:

- The tile is rotated. This is the default outcome and counts as an action in the game.
- The tile is moved and swaps with the gap. This happens if the tile tapped is directly adjacent to the gap. This only counts as an action if the tile moved could have been rotated. For example the empty tile (with no pipes on it) can not be rotated and thus can be moved around without increasing the action count.
- Nothing happens. This happens if the tile cannot be rotated and it is not directly adjacent to the gap. Obviously, this does not count as an action. The tap can also be considered invalid as it does not change the level.

The solver performs a brute-force search on all possible taps. That is, all possible combinations of taps up to a certain amount (e. g. up to 3 taps) are tested. There are, however, a few cases where it does not make sense to check combinations with more taps. These are:

- If the current combination solves the level.
- If the next tap would be invalid.
- If the next tap would rotate a tile for the fourth time, thus returning it to its initial rotation.

The solver generates the optimal solutions in terms of actions and taps. For some levels there are multiple solutions requiring the same amount of actions and taps. In these cases all of them are reported.

-----

The first version of the solver used a breadth-first search, i. e. first all taps on a single tile are checked, then all combinations of 2 taps are checked, then all combinations of 3 taps, etc.

However this approach had two problems:

1. That version of the search finds solutions with the minimal amount of taps first. However, some levels have solutions that use more taps than other solutions but still generate less actions.
2. Breadth-first search requires **a lot** of memory to remember the combinations that need to be checked. For each step that the tap limit is increased the amount of memory needed is multiplied by 24 (this is an approximation as some combinations are not checked as explained earlier). For example, after six taps up to 24^6=191,102,976 combinations need to be remembered. I got the amount of memory to store one combination down to 32 bytes. That turns out to around 6 GB total. Which is about the maximum my computer can currently handle.

To sum it up, the original reason to use breadth-first search, that it finds the optimal solution first and we can stop searching immediately, turned out to be false, since there is a difference between the optimal number of taps and the optimal number of actions and we are interested in the latter. Furthermore, the maximum number of taps is limited to 6 by the memory consumption.

It turns out that you can do a breadth-first search without using all that memory by recalculating the state of the level after some combination of taps all the time. However, since I think this would make the solver rather slow and there is no incentive to use breadth-first search anymore, I decided to switch to depth-first search, i. e. all combinations starting with a tap on the first tile are checked first, then all starting with a tap on the second tile, etc.

Usage
=====

All operations are performed in the 'data/' directory relative to the current working directory, which should already exist and contain some reference images for the solver inside 'data/reference/'.

Screenshots of the levels should be placed inside the 'data/raw/' directory.

During the preparation phase the program extracts the level number from the screenshots and places a normalized version of the level area in an appropriately named file inside the 'data/levels/' directory.

During the solving phase the program attempts to solve the levels and puts solutions in the 'data/solutions/' directory.

Options:

 Option                     | Short Option | Description
----------------------------|--------------|------------
 --help                     |              | Prints help message.
 --log                      | -l           | Logs statistics about the solving phase into a log file inside 'data/logs/'.
 --unsolved                 | -u           | Solves only those levels for which it cannot already find a solution inside 'data/solutions/'.
 --prepare _filename_ ...   | -p           | Prepares the specified files.  Must be followed by one or more filenames. Filenames must not include 'data/raw/'.
 --prepare-all              | -P           | Prepares all files in 'data/raw/'.
 --solve _X **or** X‑Y_ ... | -s           | Solves the specified levels. X and Y are level numbers between 1 and 500. Examples: 42, 1-100, etc.
 --solve-all                | -S           | Solves all levels.
 --handle _filename_ ...    | -h           | Prepares the specified files and solves the levels they contain.
 --handle-all               | -H           | Handles all files in 'data/raw/'.

Dependencies
============

The project makes use of several [Boost libraries](http://www.boost.org) (Program Options, Filesystem, Interval Container Library and some others).

stb_image.h and stb_image_write.h are used for image io ([link](https://github.com/nothings/stb)).

Compiling
=========

The project is currently compiled via the command line. On Windows, using Microsoft Visual Studio, this command is used to compile it:

    cl /Fec500 /EHsc /I "%boost_root%" ..\source\*.cpp /link /LIBPATH:"%boost_root%\lib"

License
=======

The screenshots and reference images are © 2015 Palestone Software.

The source code is licensed under the MIT License.