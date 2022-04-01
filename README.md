# Operating-Systems

## Project 1:
Developed an app for extracting data and statistics for a secretary's office.


Main Objectives:
  - Familiarization with Unix OS.
  - Implementation of complex data structures (HashTables and Inverted Index)
  - Time efficent operations (O(1))
  - No Data duplication

## Project 2:
Developed an app that finds all the prime numbers inside a given range. The app implements a flexible tree with processes in order to find those numbers:
  - At first the process hierarchy is created using fork system calls.
  - Nodes at different hierarchy levels execute different and independent executables that communicate using Pipes and Signals.

Main Objectives:
  - Unix System Calls
  - Process Creation using system calls.
  - Management and Synchronization of the new processes.
  - Communication between processes using Pipes and Signals.

## Project 3:
Developed an app that independent programms run concurrently and simulate a "Salad Creation" inside a restaurant kitchen.

Main Objectives:
  - Using POSIX Semaphores in order to synchronize and achieve a successful cooperation between the independent processes and use of Shared Memory segment.
  - Every proccess attaches the same shared memory segment in order to access variables/data structures that they need and also "communicate" with the other processes.

## Project 4:
Developed an app used for "quick copying" file/directory hierarchies when it is perfomed multiple times. For example, when used for backup files only the new or modified files are copied.
Also, the program supports the existance of Soft Links or Hard Links to files.

Main Objectives:
  - Unix File System Manipulation
  - Soft Links and Hard Links
