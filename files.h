#ifndef files_h
#define files_h

// Gets CWD from the OS and writes it to `cwd`. 1024 characters limit.
void get_current_directory(char *cwd);

// Returns full path of the passed `relativePath` by prepending the CWD and HTTP
// folder.
char *get_full_path(char *relativePath);

// Checks if the given `folderPath` directory is a directory. Returns 0 if it
// is, 1 if it isn't, or some error has occurred.
int check_dir(const char *folderPath);

// Checks if the given `filePath` file exists. Return 0 if it does, 1 if it
// doesn't.
int check_file(const char *filePath);

// Reads file at the given `filePath` and writes its contents to `buffer`,
// returning the length of the content in `filelen`.
int read_file_bytes(const char *filePath, char **buffer, long *filelen);

// Gets the current working directory and stores it in a global variable.
// This is meant to be executed only once in main(), to eliminate any
// possibility of an incorrect working directory. Also sets the HTTP folder's
// path in another global variable.
void set_cwd(char *newCWD);

#endif