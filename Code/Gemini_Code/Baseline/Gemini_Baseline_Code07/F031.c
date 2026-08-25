// F031.c
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
    char *name;
    char *note;
} Record31;

int main() {
    Record31 *store = NULL;
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
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "BATCH") == 0) {
            int batch_n = 0;
            if (sscanf(p, "%d", &batch_n) == 1) {
                char **b_names = malloc(batch_n * sizeof(char *));
                char **b_notes = malloc(batch_n * sizeof(char *));
                int valid = 1;
                for (int i = 0; i < batch_n; i++) {
                    char *b_line = read_line(stdin);
                    if (!b_line) {
                        valid = 0;
                        break;
                    }
                    char b_name[256];
                    int b_n = 0;
                    if (sscanf(b_line, "%255s%n", b_name, &b_n) == 1) {
                        char *b_note = b_line + b_n;
                        while (*b_note == ' ' || *b_note == '\t') b_note++;
                        b_names[i] = strdup(b_name);
                        b_notes[i] = strdup(b_note);
                    } else {
                        valid = 0;
                    }
                    free(b_line);
                }
                if (valid) {
                    for (int i = 0; i < batch_n; i++) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            store = realloc(store, capacity * sizeof(Record31));
                        }
                        store[count].name = b_names[i];
                        store[count].note = b_notes[i];
                        count++;
                    }
                    free(b_names);
                    free(b_notes);
                } else {
                    for (int i = 0; i < batch_n; i++) {
                        if (i < count) {
                            free(b_names[i]);
                            free(b_notes[i]);
                        }
                    }
                    free(b_names);
                    free(b_notes);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(store[found].name);
                    free(store[found].note);
                    for (int i = found; i < count - 1; i++) {
                        store[i] = store[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        printf("%s\n", store[i].note);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
