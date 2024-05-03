#ifndef string_util_h
#define string_util_h

char *trim(char *s);
char *sgets(char *s, int n, const char **strp);
unsigned short strsplit(const char **strp, char delim, char ***output);
void strsplit_free(char ***arr, unsigned short size);
int strjoin(char ***arr, int startPos, int endPos, char delim, char **output);
int str_is_number(char *str);

#endif