#ifndef files_h
#define files_h

void get_current_directory(char *cwd);
char *get_full_path(char *relativePath);
int check_dir(const char *folderPath);
int check_file(const char *filePath);
int read_file_bytes(const char *filePath, char **buffer, long *filelen);

#endif