// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char **values;
    int val_count;
    int val_capacity;
} IndexEntry;

int main() {
    IndexEntry *entries = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", entries[i].key);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char k[256], v[256];
            if (sscanf(p, "%255s %255s", k, v) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, k) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        entries = realloc(entries, capacity * sizeof(IndexEntry));
                    }
                    entries[count].key = strdup(k);
                    entries[count].values = NULL;
                    entries[count].val_count = 0;
                    entries[count].val_capacity = 0;
                    found = count;
                    count++;
                }
                if (entries[found].val_count >= entries[found].val_capacity) {
                    entries[found].val_capacity = entries[found].val_capacity == 0 ? 4 : entries[found].val_capacity * 2;
                    entries[found].values = realloc(entries[found].values, entries[found].val_capacity * sizeof(char *));
                }
                entries[found].values[entries[found].val_count++] = strdup(v);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char k[256], v[256];
            if (sscanf(p, "%255s %255s", k, v) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, k) == 0) {
                        int v_found = -1;
                        for (int j = 0; j < entries[i].val_count; j++) {
                            if (strcmp(entries[i].values[j], v) == 0) {
                                v_found = j;
                                break;
                            }
                        }
                        if (v_found != -1) {
                            free(entries[i].values[v_found]);
                            for (int j = v_found; j < entries[i].val_count - 1; j++) {
                                entries[i].values[j] = entries[i].values[j + 1];
                            }
                            entries[i].val_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char k[256];
            if (sscanf(p, "%255s", k) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, k) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(entries[found].key);
                    for (int j = 0; j < entries[found].val_count; j++) {
                        free(entries[found].values[j]);
                    }
                    free(entries[found].values);
                    for (int i = found; i < count - 1; i++) {
                        entries[i] = entries[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char k[256];
            if (sscanf(p, "%255s", k) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(entries[i].key, k) == 0) {
                        for (int j = 0; j < entries[i].val_count; j++) {
                            printf("%s%s", entries[i].values[j], (j == entries[i].val_count - 1) ? "" : " ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(entries[i].key);
        for (int j = 0; j < entries[i].val_count; j++) {
            free(entries[i].values[j]);
        }
        free(entries[i].values);
    }
    free(entries);
    return 0;
}
