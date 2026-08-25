// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Record *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s: %s\n", list[i].name, list[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(rest);
            char **batch_lines = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                batch_lines[i] = read_line(stdin);
                if (!batch_lines[i] || !strchr(batch_lines[i], ' ')) {
                    valid = 0;
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    char *p = strchr(batch_lines[i], ' ');
                    *p = '\0';
                    char *name = batch_lines[i];
                    char *note = p + 1;
                    list = realloc(list, (count + 1) * sizeof(Record));
                    list[count].name = strdup(name);
                    list[count].note = strdup(note);
                    count++;
                }
            }
            for (int i = 0; i < n; i++) {
                if (batch_lines[i]) free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Record));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    printf("%s\n", list[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    return 0;
}