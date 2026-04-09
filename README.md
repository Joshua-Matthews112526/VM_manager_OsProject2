# VM_manager_OsProject2

Virtual Memory Manager Project
Operating Systems

Name: Joshua Matthews
Course: CS 30200-001
Instructor: Professor Zhang
Date: 04-08-26

Project Description:

This project simulates a Virtual Memory Manager that translates logical
addresses into physical addresses using:

- A Page Table (256 entries)
- A TLB (Translation Lookaside Buffer) with 16 entries
- Demand Paging using BACKING_STORE.bin

The program reads logical addresses from an input file (addresses.txt),
extracts the page number and offset from each 16-bit logical address,
checks the TLB first, then the page table, and handles page faults by
loading the required page from BACKING_STORE.bin into physical memory.

For each logical address, the program outputs:

1. Virtual (logical) address
2. Physical address
3. The signed byte value stored at that physical address

At the end of execution, the program also reports:

- Number of translated addresses
- Number of page faults
- Page fault rate
- Number of TLB hits
- TLB hit rate



Program Design Summary:


Specifications implemented:

- Logical address space: 65,536 bytes (16-bit addresses)
- Page size: 256 bytes
- Frame size: 256 bytes
- Number of pages: 256
- Number of frames: 256
- Physical memory size: 65,536 bytes
- TLB size: 16 entries
- TLB replacement policy: FIFO
- Page fault handling: Demand paging from BACKING_STORE.bin

Implementation details:

- The lower 16 bits of each logical address are used.
- The upper 8 bits represent the page number.
- The lower 8 bits represent the offset.
- If a page is not in the TLB or page table, a page fault occurs.
- The missing page is loaded from BACKING_STORE.bin into the next free frame.
- Since physical memory size equals virtual memory size, no page replacement
  is required.



Files Included:


- vm_manager.cpp              -> C++ source code
- vm_manager.exe              -> Compiled executable
- vm_manager.o                -> Object code file
- addresses.txt               -> Input file of logical addresses
- BACKING_STORE.bin           -> Backing store binary file
- output.txt                  -> Program output generated from addresses.txt
- correct.txt                 -> Provided correct output for verification
- README.txt                  -> This file



How to Compile (Windows CMD):

Open Command Prompt in a windows OS and run:

Step 1.) cd path of the folder

in my case: cd C:\Users\tim\Downloads\OperatingSystems\Project2

Step 2.) g++ vm_manager.cpp -o vm_manager.exe



How to Create Object Code (Windows CMD):

To generate the object code file:

Step 1.) g++ -c vm_manager.cpp -o vm_manager.o




How to Run the Program (Windows CMD):

Run the executable with the logical address file as the argument:

Step 1.) vm_manager.exe addresses.txt



How to Save Output to a File (Windows CMD):


To save the program output to a text file:

Step 1.) vm_manager.exe addresses.txt > output.txt

This creates output.txt containing:

- All translated address results
- Final statistics



Notes:


- BACKING_STORE.bin must be in the same directory as the executable.
- addresses.txt must also be in the same directory.
- The program uses FIFO replacement for the 16-entry TLB.
- Physical memory is large enough to hold all pages, so no frame replacement
  is needed.



Verification:

The generated output in output.txt can be compared against correct.txt
to verify that the address translations and values are correct.
