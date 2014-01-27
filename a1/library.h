#ifndef __LIBRARY_H_INCLUDED
# define __LIBRARY_H_INCLUDED

#include <vector>
#include <cstdio>

#define SCHEMA_ATTRIBUTE_LEN 10
#define SCHEMA_ATTRIBUTES_NUM 100

typedef const char* V;
typedef std::vector<V> Record;
// The byte offset position in a file
typedef int Offset;

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);


/**
 * Deserializes from `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);


/**
* Page
*/
typedef struct {
    void *data;
    int page_size;
    int slot_size;
    int capacity;
} Page;

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);

/**
 * Read a record from the page from a given slot.
 */
bool read_fixed_len_page(Page *page, int slot, Record *r);


/**
 * Heap file
 */
typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

/*
 * PageID, unique for a page within a heapfile
 * We use the byte offset of the page as its page id.
 * This is the same as the page_offset data members in
 * RecordID and DirectoryRecord.
 */
typedef Offset PageID;

/** 
 * Data structure defining the ID of a record. 
 * The RecordID must be unique given a heapfile
 */
typedef struct {
    Offset page_offset;
    int slot;
} RecordID;

// A number used to check if the deserialized heapfile is non-empty
#define DIRECTORY_RECORD_SIGNATURE 314159

// The first directory's byte offset, should be 0
#define FIRST_DIRECTORY_OFFSET 0

/**
 * Initalize a heapfile to use the file and page size given.
 * If the file is a new heapfile, create the first directory page
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

/**
 * Locate the first directory of a heapfile
 */
void locate_directory(Heapfile *heapfile, Page *directory);
/**
 * Locate the header of a directory
 */ 
void locate_header(Page *directory, Record *header);

/**
 * The directory iterator for a heapfile of multiple directories
 */
class DirectoryIterator {
	public:
		DirectoryIterator(Heapfile* heapf);
		~DirectoryIterator();
		bool hasNext();

		// Return the next directory page
		Page* next();

	private:
		Heapfile* heapfile;
};

/* Page iterator class for a directory 
 * Used to iterate through all pages in a directory
 */
class PageIterator {
	public:
		PageIterator(Heapfile* heapfile, Page* directory);
		~PageIterator();
		bool hasNext();
		
		// Return the next data page
		Page* next();

	private:
		Heapfile* heapfile;
};

/* Record iterator class for iterating through 
 * all records in the heap file
 */
class RecordIterator {
	public:
	    RecordIterator(Heapfile *heapfile);
	    ~RecordIterator();

	    // Get the next non-empty record in the heap
	    Record next();
	    
	    // Check if the heap has anymore non-empty record
	    bool hasNext();

	private:
		// The heap file we are iterating
		Heapfile* heapfile;
};

#endif
