#include <sec_api/string_s.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#elif __unix__
#include <unistd.h>
#endif

#include "files.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>

#define FILE_PATH_LENGTH_LIMIT 2048

char g_CWD[FILE_PATH_LENGTH_LIMIT] = {};
char g_HTMLDIR[FILE_PATH_LENGTH_LIMIT] = {};

void get_current_directory(char *cwd) {
#ifdef _WIN32
    GetCurrentDirectory(FILE_PATH_LENGTH_LIMIT, cwd);
#elif __unix__
    getcwd(cwd, FILE_PATH_LENGTH_LIMIT);
#endif
}

char *get_full_path(char *relativePath) {
  char *concatPath = malloc(FILE_PATH_LENGTH_LIMIT); // Allocate memory on the heap

  if (concatPath == NULL) {
    return NULL;
  }

  strcpy(concatPath, g_HTMLDIR);
  strcat(concatPath, relativePath);
  return concatPath;
}

int check_dir(const char *fullFolderPath) {
  DIR *dirptr = opendir(fullFolderPath);

  if (dirptr) {
    closedir(dirptr);
    return 0;
  } else if (ENOENT == errno) {
    return 1;
  } else {
    printf("Unknown error while checking directory\n");
    return 1;
  }
}

int check_file(const char *filePath) {
  if (access(filePath, F_OK) == 0) {
    return 0;
  } else {
    return 1;
  }
}

int read_file_bytes(const char *filePath, char **buffer, long *filelen) {
  FILE *fileptr;

  // Seek to EOF, get the current offset to get file's length in binary, rewind to pos0
  fileptr = fopen(filePath, "rb");
  fseek(fileptr, 0, SEEK_END);
  *filelen = ftell(fileptr);
  rewind(fileptr);

  // Create large enough buffer, read the entire file into buffer, close file
  if (*buffer == NULL) {
    *buffer = malloc(*filelen * sizeof(char));
  } else {
    *buffer = realloc(*buffer, *filelen * sizeof(char));
  }

  if (*buffer == NULL) {
    return 1;
  }
  
  fread(*buffer, *filelen, 1, fileptr);
  fclose(fileptr);
  return 0;
}

void set_cwd(char *newCWD) {
  strcpy_s(g_CWD, strlen(newCWD), newCWD);
  sprintf(g_HTMLDIR, "%s/html", newCWD);
}