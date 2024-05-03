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

void get_current_directory(char *cwd) {
#ifdef _WIN32
    GetCurrentDirectory(MAX_PATH, cwd);
#elif __unix__
    getcwd(cwd, 1024);
#endif
}

char *get_full_path(char *relativePath) {
  char cwd[1024];
  get_current_directory(cwd);
  strcat(cwd, relativePath);

  unsigned long long strLen = strlen(cwd) + 1;

  char *result = malloc(sizeof(char) * strLen);
  strcpy_s(result, strLen, cwd);

  return result;
}

int check_dir(const char *fullFolderPath) {
  DIR *dir = opendir(fullFolderPath);

  if (dir) {
    closedir(dir);
    return 0;
  } else if (ENOENT == errno) {
    return 1;
  } else {
    printf("Unknown error while checking directory\n");
    return 1;
  }
}

int check_file(const char *filePath) {
  struct stat buffer;
  return stat(filePath, &buffer) == 0 ? true : false;
}