// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *note;
} Rec;

int main() {
    Rec *recs = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", recs[i].name, recs[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(args);
            char **batch_lines = malloc(sizeof(char*) * n);
            int all_valid = 1;
            for (int i = 0; i < n; i++) {
                batch_lines[i] = read_line(stdin);
                if (!batch_lines[i]) {
                    all_valid = 0;
                } else {
                    char *sp = strchr(batch_lines[i], ' ');
                    if (!sp) all_valid = 0;
                }
            }
            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    char *sp = strchr(batch_lines[i], ' ');
                    *sp = '\0';
                    recs = realloc(recs, sizeof(Rec) * (count + 1));
                    recs[count].name = strdup(batch_lines[i]);
                    recs[count].note = strdup(sp + 1);
                    count++;
                }
            }
            for (int i = 0; i < n; i++) {
                if (batch_lines[i]) free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recs[i].name, args) == 0) {
                    free(recs[i].name);
                    free(recs[i].note);
                    for (int j = i; j < count - 1; j++) {
                        recs[j] = recs[j + 1];
                    }
                    count--;
                    if (count == 0) { free(recs); recs = NULL; }
                    else { recs = realloc(recs, sizeof(Rec) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recs[i].name, args) == 0) {
                    printf("%s\n", recs[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recs[i].name);
        free(recs[i].note);
    }
    free(recs);
    return 0;
}
