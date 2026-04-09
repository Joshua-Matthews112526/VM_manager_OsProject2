#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Constants 
static const int PAGE_SIZE       = 256;   // bytes per page / frame
static const int NUM_FRAMES      = 256;   // frames in physical memory
static const int NUM_PAGES       = 256;   // entries in page table
static const int TLB_SIZE        = 16;    // TLB entries
static const int ADDR_MASK       = 0xFFFF;
static const int PAGE_MASK       = 0xFF00;
static const int OFFSET_MASK     = 0x00FF;
static const int PAGE_SHIFT      = 8;

// Physical Memory 
signed char physicalMemory[NUM_FRAMES][PAGE_SIZE];
int nextFreeFrame = 0;

// Page Table 
// -1 means the page has not been loaded yet (invalid)
int pageTable[NUM_PAGES];

// TLB (Translation Lookaside Buffer) 
struct TLBEntry {
    int  page;
    int  frame;
    bool valid;
};
TLBEntry tlb[TLB_SIZE];
int tlbNext = 0;   // FIFO replacement index

// Statistics 
int totalAccesses = 0;
int tlbHits       = 0;
int pageFaults    = 0;

// Backing Store 
FILE* backingStore = nullptr;


// Initialize all data structures

void initialize() {
    // Mark every page table entry as invalid
    for (int i = 0; i < NUM_PAGES; i++)
        pageTable[i] = -1;

    // Mark every TLB entry as invalid
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page  = -1;
        tlb[i].frame = -1;
        tlb[i].valid = false;
    }

    // Zero out physical memory
    memset(physicalMemory, 0, sizeof(physicalMemory));
}


// TLB lookup — returns frame number on hit, -1 on miss

int tlbLookup(int pageNum) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].page == pageNum) {
            return tlb[i].frame;
        }
    }
    return -1;
}


// TLB insert — FIFO replacement

void tlbInsert(int pageNum, int frameNum) {
    tlb[tlbNext].page  = pageNum;
    tlb[tlbNext].frame = frameNum;
    tlb[tlbNext].valid = true;
    tlbNext = (tlbNext + 1) % TLB_SIZE;
}


// Handle a page fault: load page from backing store into the next free frame,
// then update the page table and TLB.

int handlePageFault(int pageNum) {
    pageFaults++;

    // Seek to the correct page in the backing store
    if (fseek(backingStore, pageNum * PAGE_SIZE, SEEK_SET) != 0) {
        std::cerr << "ERROR: fseek failed for page " << pageNum << "\n";
        exit(EXIT_FAILURE);
    }

    // Read 256 bytes into the next free frame
    int frame = nextFreeFrame;
    size_t bytesRead = fread(physicalMemory[frame], sizeof(signed char), PAGE_SIZE, backingStore);
    if ((int)bytesRead != PAGE_SIZE) {
        std::cerr << "ERROR: fread returned " << bytesRead
                  << " bytes (expected " << PAGE_SIZE << ") for page " << pageNum << "\n";
        exit(EXIT_FAILURE);
    }

    // Update page table
    pageTable[pageNum] = frame;

    // Update TLB (FIFO)
    tlbInsert(pageNum, frame);

    nextFreeFrame++;   // Since physical memory == virtual space, this never overflows
    return frame;
}


// Translate one logical address → physical address, return the byte value

void translateAddress(int logicalAddr, int& physicalAddr, signed char& value) {
    totalAccesses++;

    // Step 1 mask to 16 bits, extract page number and offset
    int masked  = logicalAddr & ADDR_MASK;
    int pageNum = (masked & PAGE_MASK) >> PAGE_SHIFT;
    int offset  = masked & OFFSET_MASK;

    // Step 2  consult TLB
    int frameNum = tlbLookup(pageNum);

    if (frameNum != -1) {
        // TLB hit
        tlbHits++;
    } else {
        // TLB miss, consult page table
        frameNum = pageTable[pageNum];

        if (frameNum == -1) {
            // Page fault, load from backing store
            frameNum = handlePageFault(pageNum);
        } else {
            // Page table hit, still update TLB so future accesses are faster
            tlbInsert(pageNum, frameNum);
        }
    }

    // Step 3 form physical address and read value
    physicalAddr = (frameNum << PAGE_SHIFT) | offset;
    value        = physicalMemory[frameNum][offset];
}


// main function 

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " addresses.txt\n";
        return EXIT_FAILURE;
    }

    // Open the backing store
    backingStore = fopen("BACKING_STORE.bin", "rb");
    if (!backingStore) {
        std::cerr << "ERROR: Cannot open BACKING_STORE.bin\n";
        return EXIT_FAILURE;
    }

    // Open the logical address file
    std::ifstream addrFile(argv[1]);
    if (!addrFile.is_open()) {
        std::cerr << "ERROR: Cannot open " << argv[1] << "\n";
        fclose(backingStore);
        return EXIT_FAILURE;
    }

    initialize();

    int logicalAddr;
    while (addrFile >> logicalAddr) {
        int        physicalAddr;
        signed char value;

        translateAddress(logicalAddr, physicalAddr, value);

        std::cout << "Virtual address: "  << logicalAddr
                  << " Physical address: " << physicalAddr
                  << " Value: "            << static_cast<int>(value)
                  << "\n";
    }

    //  Print statistics 
    std::cout << "\n";
    std::cout << "Number of Translated Addresses = " << totalAccesses << "\n";
    std::cout << "Page Faults = "     << pageFaults << "\n";
    std::cout << "Page Fault Rate = " << (1.0 * pageFaults / totalAccesses) << "\n";
    std::cout << "TLB Hits = "        << tlbHits << "\n";
    std::cout << "TLB Hit Rate = "    << (1.0 * tlbHits / totalAccesses) << "\n";

    addrFile.close();
    fclose(backingStore);
    return EXIT_SUCCESS;
}
