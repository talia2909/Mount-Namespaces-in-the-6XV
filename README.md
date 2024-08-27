
# Implementation of Mount Namespaces in the 6XV Operating System

## Project Overview

This project focuses on implementing and debugging mount namespaces in the 6XV operating system. The task involves identifying and fixing a bug that causes a kernel panic when running filesystem-related operations within the mount namespace.

### Project Goals:
- Gain familiarity with the 6XV operating system.
- Understand the practical aspects of container implementation.
- Learn about the implementation of `namespace mount` in the 6XV operating system.
- Fix a kernel panic bug related to mounted filesystems.

## Key Project Files:
- `mnt_ns.c`: Implementation and bug fixes for the `nsleave_mount` function, which manages the cleanup and exit procedures for mount namespaces.

## Execution Steps:
1. **Running the 6XV Operating System**: 
   - Follow the steps in the course material to run the 6XV operating system.

2. **Reproducing the Bug**:
   - Run the `mounttest` program from the command line in 6XV **twice**. The bug will manifest during the second run, causing a kernel panic.
   - Note that in the correct solution, the bug does not occur, even after multiple runs.

3. **Bug Description**:
   - The kernel panic occurs due to the system being unable to find a `list_mount` object to allocate.
   - The issue lies in the `nsleave_mount` function within `mnt_ns.c`, which fails to properly clean up and update the state when the last process leaves the `namespace mount`.

4. **Bug Fix**:
   - The fix involves adding three specific actions to the nearly empty `nsleave_mount` function, including a call to an existing function within the same file that is appropriately named for its role.
   - Ensure that the function correctly empties the existing mounts and updates the state when the last process leaves the namespace. If a non-final process leaves, the registration should be adjusted accordingly.

5. **Final Testing**:
   - After fixing the bug, run `make clean` and `make qemu` again to ensure the system no longer crashes.
   - Perform regression testing to verify that all tests pass successfully.

6. **Automation with Expect**:
   - If not already installed, install the `expect` automation scripting language:
     ```bash
     sudo apt-get install expect
     ```
   - Run the automated tests:
     ```bash
     ./runtests.exp my.log
     echo $?
     ```
   - Ensure that the status is `0`, indicating all tests passed successfully.

## Conclusion
This project provided valuable experience in debugging kernel-level issues and enhancing the stability of the 6XV operating system by correctly implementing the cleanup procedures for mount namespaces.

