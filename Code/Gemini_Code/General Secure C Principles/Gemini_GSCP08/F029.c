// F029.c
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
    char *key;
    char *value;
} DupRecord;

int main(void) {
    DupRecord *arr = NULL;
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
            if (strcmp(cmd, "DEDUP") == 0) {
                size_t i = 0;
                while (i < count) {
                    int duplicate = 0;
                    for (size_t j = 0; j < i; j++) {
                        if (strcmp(arr[j].key, arr[i].key) == 0) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (duplicate) {
                        free(arr[i].key);
                        free(arr[i].value);
                        for (size_t j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].key, arr[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = args;
            char *p2 = strchr(key, ' ');
            if (p2) {
                *p2 = '\0';
                char *val = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    DupRecord *narr = realloc(arr, ncap * sizeof(DupRecord));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].key = strdup(key);
                    arr[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = args;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    return 0;
}
