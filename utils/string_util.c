#include <ctype.h>
#include <stdio.h>
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