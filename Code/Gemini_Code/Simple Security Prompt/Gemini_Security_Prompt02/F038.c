// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *encoded;
} RLEData;

int main() {
    RLEData *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "STORE ", 6) == 0) {
            char *name = line + 6;
            char *space = strchr(name, ' ');
            if (space) {
                *space = '\0';
                char *encoded = space + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(RLEData));
                }
                arr[count].name = strdup(name);
                arr[count].encoded = strdup(encoded);
                count++;
            }
        } else if (strncmp(line, "DECODE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *p = arr[i].encoded;
                    int valid = 1;
                    while (*p) {
                        if (*p >= '0' && *p <= '9') {
                            int cnt = 0;
                            while (*p >= '0' && *p <= '9') {
                                cnt = cnt * 10 + (*p - '0');
                                if (cnt > 10000) {
                                    valid = 0;
                                    break;
                                }
                                p++;
                            }
                            if (!valid || *p == '\0') {
                                valid = 0;
                                break;
                            }
                            char c = *p;
                            p++;
                            for (int k = 0; k < cnt; k++) {
                                if (c == ' ') putchar('_');
                                else putchar(c);
                            }
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    if (valid) putchar('\n');
                    else printf("INVALID\n");
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
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
