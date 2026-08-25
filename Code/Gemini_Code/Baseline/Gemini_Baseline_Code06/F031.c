// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    char *note;
} BatchRecord;

int main() {
    BatchRecord *db = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            char **batch_lines = malloc(n * sizeof(char *));
            int all_valid = 1;
            for (int i = 0; i < n; i++) {
                batch_lines[i] = read_line(stdin);
                if (!batch_lines[i]) {
                    all_valid = 0;
                } else {
                    char *p = batch_lines[i];
                    while (*p && *p != ' ') p++;
                    if (*p != ' ' || *(p + 1) == '\0') {
                        all_valid = 0;
                    }
                }
            }
            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    char *p = batch_lines[i];
                    char *name = p;
                    while (*p && *p != ' ') p++;
                    *p = '\0';
                    char *note = p + 1;
                    db = realloc(db, (count + 1) * sizeof(BatchRecord));
                    db[count].name = strdup(name);
                    db[count].note = strdup(note);
                    count++;
                }
            }
            for (int i = 0; i < n; i++) {
                if (batch_lines[i]) free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    free(db[i].name);
                    free(db[i].note);
                    for (int j = i; j < count - 1; j++) {
                        db[j] = db[j + 1];
                    }
                    count--;
                    db = realloc(db, count * sizeof(BatchRecord));
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(db[i].name, name) == 0) {
                    printf("%s\n", db[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", db[i].name, db[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(db[i].name);
        free(db[i].note);
    }
    free(db);
    return 0;
}
