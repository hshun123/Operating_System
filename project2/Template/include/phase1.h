// Add Guard to header file (google what are guards)
// Function prototypes to
//			Traverse the Folder
//			Partition the text file paths to 'm' files
#ifndef PAHSE_1
#define PHASE_1

void print_softlinks(const char *curr_dir, char *entry_name);

int traverse(const char *curr_dir);

void process (const char *curr_dir, int m);

#endif
