// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *category;
    char *text;
} FilteredNote;

int main(void) {
    FilteredNote *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "PRINT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].category, arr[i].text);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = args;
            char *p2 = strchr(cat, ' ');
            if (p2) {
                *p2 = '\0';
                char *text = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    FilteredNote *narr = realloc(arr, ncap * sizeof(FilteredNote));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].category = strdup(cat);
                    arr[count].text = strdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = args;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) == 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = args;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].category, cat) != 0) {
                    free(arr[i].category);
                    free(arr[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = args;
            for (size_t i = 0; i < count; i++) {
                if (strstr(arr[i].text, word) != NULL) {
                    printf("%s %s\n", arr[i].category, arr[i].text);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].category);
        free(arr[i].text);
    }
    free(arr);
    return 0;
}
