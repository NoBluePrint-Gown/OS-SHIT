# Process Dispatcher Simulation

## Overview

This project implements a process dispatcher simulator that demonstrates CPU scheduling algorithms. The dispatcher manages processes in a ready queue and schedules them for execution using different scheduling algorithms.

## Features

- **Process Control Block (PCB)** structure to track process information
- **Ready Queue** management for waiting processes
- **Scheduling Algorithms:**
  - First-Come-First-Serve (FCFS)
  - Shortest Job First (SJF) with FCFS tie-breaking
- **Process States:** READY, RUNNING, TERMINATED
- **Time-based simulation** with tick-by-tick execution
- **Context switching** between processes

## Prerequisites

- **GCC Compiler** (version 4.8 or later)
- **Make** (optional, for using Makefile)
- **Linux/Unix/MacOS** environment (can also run on Windows with WSL or MinGW)

## Building the Project

### Using Make (Recommended)

```bash
# Build the project
make

# Build with debug symbols
make debug

# Build optimized release version
make release

# Build with AddressSanitizer for memory checking
make sanitize
