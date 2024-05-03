#include <ctype.h>
#include <sec_api/string_s.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *trim(char *str) {
  char *end;

  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0) // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

// Stops at null terminator or newline
char *sgets(char *s, int n, const char **strp) {
  if (**strp == '\0' || **strp == '\n' || isspace(**strp))
    return NULL;
  int i;
  for (i = 0; i < n - 1; ++i, ++(*strp)) {
    s[i] = **strp;
    if (**strp == '\0') {
      break;
    }

    if (**strp == '\n') {
      s[i + 1] = '\0';
      ++(*strp);
      break;
    }
  }
  if (i == n - 1)
    s[i] = '\0';
  return s;
}

unsigned short strsplit(const char **strp, char delim, char ***output) {
  char buff[512];
  unsigned short arrSize = 0;
  unsigned short strlen = 0;
  memset(buff, 0, sizeof(buff));

  /*
  Read each character of the given string and save it in the buffer. If the
  delimiter is encountered, take what is in the buffer, malloc and copy it to a
  new string and add it to the output array.
  */
  while (**strp != '\n' && **strp != '\0') {
    if (**strp == delim) {
      char *splitStr = malloc(sizeof(char) * strlen + 1);
      strcpy_s(splitStr, strlen + 1, buff);

      printf("%s\n", splitStr);

      if (output == NULL) {
        *output = malloc(sizeof(char *) * (arrSize + 1));
      } else {
        *output = realloc(*output, sizeof(char *) * (arrSize + 1));
      }

      (*output)[arrSize] = splitStr;

      strlen = 0;
      memset(buff, 0, strlen);
      arrSize++;
    } else {
      buff[strlen] = **strp;
      strlen++;
    }

    ++(*strp);
  }

  /*
  This is for when the string is complete, but there are items left in the
  buffer; in such cases, write the remaining characters in the buffer to the
  result array.
  */
  if (strlen > 0) {
    char *splitStr = malloc(sizeof(char) * strlen + 1);
    strcpy_s(splitStr, strlen + 1, buff);

    if (output == NULL) {
      *output = malloc(sizeof(char *) * (arrSize + 1));
    } else {
      *output = realloc(*output, sizeof(char *) * (arrSize + 1));
    }

    (*output)[arrSize] = splitStr;

    strlen = 0;
    memset(buff, 0, strlen);
    arrSize++;
  }

  return arrSize;
}

/*
Free the array returned from strsplit
*/
void strsplit_free(char ***arr, unsigned short size) {
  for (int i = 0; i < size; i++) {
    free((*arr)[i]);
    (*arr)[i] = NULL;
  }

  free(*arr);
  *arr = NULL;

  arr = NULL;
}

/*
Join the different elements of a given string array into a single output string
*/

int strjoin(char ***arr, int startPos, int endPos, char delim, char **output) {
  char strBuff[1024];
  memset(strBuff, 0, sizeof(strBuff));
  int strReader = 0;
  int arrReader = startPos;

  if (startPos > endPos || startPos == endPos || startPos < 0 || endPos < 0)
    return 1;

  while (arrReader <= endPos) {
    char *currentStr = (*arr)[arrReader];
    printf("Current: %s\n", currentStr);

    if (arrReader != startPos) {
      strBuff[strReader] = delim;
      strReader++;
    }

    char *currentChar = currentStr;
    while (*currentChar != '\0') {
      strBuff[strReader] = *currentChar;
      strReader++;
      currentChar++;
    }

    arrReader++;
  }

  if (*output == NULL) {
    *output = malloc(strReader + 1);
  } else {
    *output = realloc(*output, strReader + 1);
  }

  strcpy_s(*output, strReader + 1, strBuff);

  return 0;
}

/*
Check if target string includes the given substring
*/
int str_includes(char **target, char *substr) {
  unsigned long long targetLen = strlen(*target);
  unsigned long long substrLen = strlen(substr);

  if (substrLen > targetLen)
    return 0;

  char *left = *target;
  char *right = *target + (sizeof(char) * substrLen);

  while (*right != '\0') {
    if (strncmp(left, substr, substrLen) == 0)
      return 1;
    left++;
    right++;
  }
  return 0;
}

int str_is_number(char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') {
      return 0;
    }
  }
  return 1;
}