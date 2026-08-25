// F003.c
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
    char *expansion;
} Alias;

int main(void) {
    Alias *entries = NULL;
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
        if (strcmp(cmd, "DEFINE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *exp = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    free(entries[i].expansion);
                    entries[i].expansion = strdup(exp);
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Alias *new_entries = realloc(entries, capacity * sizeof(Alias));
                    if (new_entries) entries = new_entries;
                }
                if (count < capacity) {
                    entries[count].name = strdup(name);
                    entries[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    char *t = entries[i].expansion;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *new_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, old_name) == 0) {
                    free(entries[i].name);
                    entries[i].name = strdup(new_name);
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].name, name) == 0) {
                    free(entries[i].name);
                    free(entries[i].expansion);
                    for (size_t j = i; j < count - 1; j++) {
                        entries[j] = entries[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", entries[i].name, entries[i].expansion);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].name);
        free(entries[i].expansion);
    }
    free(entries);
    return 0;
}
