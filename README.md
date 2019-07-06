# Writr
### *A basic, distraction free, linux-based writing software.*

## How to use
Writr uses C++ and ncurses to manipulate the terminal buffer on linux distros. It has only been tested on ubuntu. To run, just type `./bin/writr` from the main directory.

If you want to compile it yourself, it uses my other project called Simple Compiler Utility (SCU). In order to use that, download the binaries for that project (just one file called `compile`), and put it in the root directory and run it. The `writr.proj` file is set up to make compiling a breeze.

You must have ncurses installed on your computer in the `/opt/` directory. If you have ncurses on your computer, but somewhere else, change the `<dir>` part of the library section in `writr.proj`.

## Current State

The current state of the project is entirely a WIP. The code in its current form is very messy and stands more as a proof of concept if anything.
