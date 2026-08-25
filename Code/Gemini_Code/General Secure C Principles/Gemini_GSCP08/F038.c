// F038.c
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
    char *encoded;
} RLERecord;

int main(void) {
    RLERecord *arr = NULL;
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
            if (strcmp(cmd, "LIST") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s\n", arr[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "STORE") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *encoded = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    RLERecord *narr = realloc(arr, ncap * sizeof(RLERecord));
                    if (narr) { arr = narr; cap = ncap; }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].encoded = strdup(encoded);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *p = arr[i].encoded;
                    int ok = 1;
                    while (*p != '\0' && ok) {
                        if (*p >= '0' && *p <= '9') {
                            char *end;
                            long cnt = strtol(p, &end, 10);
                            if (cnt <= 0 || cnt > 10000 || end == p || *end == '\0') {
                                ok = 0;
                                break;
                            }
                            char c = *end;
                            for (long j = 0; j < cnt; j++) {
                                if (c == ' ') putchar('_');
                                else putchar(c);
                            }
                            p = end + 1;
                        } else {
                            ok = 0;
                            break;
                        }
                    }
                    if (ok) {
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].encoded);
    }
    free(arr);
    return 0;
}
