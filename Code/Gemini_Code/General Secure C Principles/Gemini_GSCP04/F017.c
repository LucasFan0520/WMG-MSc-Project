// F017.c
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
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    Config *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *fline = read_line(f);
                if (!fline) break;
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = fline;
                    char *val = eq + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Config *new_entries = realloc(entries, capacity * sizeof(Config));
                        if (new_entries) entries = new_entries;
                    }
                    if (count < capacity) {
                        entries[count].key = strdup(key);
                        entries[count].value = strdup(val);
                        count++;
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].key, key) == 0) {
                    free(entries[i].value);
                    entries[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found && *key != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Config *new_entries = realloc(entries, capacity * sizeof(Config));
                    if (new_entries) entries = new_entries;
                }
                if (count < capacity) {
                    entries[count].key = strdup(key);
                    entries[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].key, key) == 0) {
                    free(entries[i].key);
                    free(entries[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        entries[j] = entries[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].key, key) == 0) {
                    printf("%s\n", entries[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", entries[i].key, entries[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].key);
        free(entries[i].value);
    }
    free(entries);
    return 0;
}
