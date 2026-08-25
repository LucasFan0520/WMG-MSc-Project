// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *encoded;
} RLEEntry;

int main(void) {
    RLEEntry *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "STORE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *encoded = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    free(entries[i].encoded);
                    entries[i].encoded = strdup(encoded);
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RLEEntry *new_entries = realloc(entries, capacity * sizeof(RLEEntry));
                    if (new_entries) entries = new_entries;
                }
                if (count < capacity) {
                    entries[count].name = strdup(name);
                    entries[count].encoded = strdup(encoded);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    char *ep = entries[i].encoded;
                    int valid = 1;
                    while (*ep && valid) {
                        if (*ep >= '0' && *ep <= '9') {
                            long long repeat = 0;
                            while (*ep >= '0' && *ep <= '9') {
                                repeat = repeat * 10 + (*ep - '0');
                                if (repeat > 10000) {
                                    valid = 0;
                                    break;
                                }
                                ep++;
                            }
                            if (valid && *ep != '\0') {
                                char ch = *ep;
                                if (ch == ' ') ch = '_';
                                for (long long r = 0; r < repeat; r++) {
                                    putchar(ch);
                                }
                                ep++;
                            } else {
                                valid = 0;
                            }
                        } else {
                            valid = 0;
                        }
                    }
                    if (valid) putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    free(entries[i].name);
                    free(entries[i].encoded);
                    for (size_t j = i; j < count - 1; j++) {
                        entries[j] = entries[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", entries[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].encoded);
    }
    free(entries);
    return 0;
}
