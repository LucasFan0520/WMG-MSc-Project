// F036.c
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
    char *name;
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *arr = NULL;
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
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *prefix = p2 + 1;
                char *colon = strchr(prefix, ':');
                if (colon) {
                    *colon = '\0';
                    long len = strtol(prefix, NULL, 10);
                    char *data = colon + 1;
                    if (len >= 0 && (size_t)len <= strlen(data)) {
                        char *data_copy = malloc(len + 1);
                        if (data_copy) {
                            strncpy(data_copy, data, len);
                            data_copy[len] = '\0';
                            if (count >= cap) {
                                size_t ncap = cap == 0 ? 4 : cap * 2;
                                PrefixedRecord *narr = realloc(arr, ncap * sizeof(PrefixedRecord));
                                if (narr) {
                                    arr = narr;
                                    cap = ncap;
                                }
                            }
                            if (count < cap) {
                                arr[count].name = strdup(name);
                                arr[count].data = data_copy;
                                count++;
                            } else {
                                free(data_copy);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    for (size_t j = 0; arr[i].data[j] != '\0'; j++) {
                        if (arr[i].data[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(arr[i].data[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].data);
    }
    free(arr);
    return 0;
}
