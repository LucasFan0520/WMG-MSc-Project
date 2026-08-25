// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            int n = atoi(p);
            int valid = 1;
            char **b_names = malloc(n * sizeof(char *));
            char **b_notes = malloc(n * sizeof(char *));
            if (n > 0 && (!b_names || !b_notes)) return 1;
            for (int i = 0; i < n; i++) {
                char *bline = NULL;
                size_t blen = 0;
                ssize_t bread = getline(&bline, &blen, stdin);
                if (bread != -1) {
                    if (bread > 0 && bline[bread - 1] == '\n') {
                        bline[bread - 1] = '\0';
                    }
                    char *bp = bline;
                    while (*bp == ' ') bp++;
                    char *name = bp;
                    char *space = strchr(name, ' ');
                    char *note = "";
                    if (space) {
                        *space = '\0';
                        note = space + 1;
                        while (*note == ' ') note++;
                    }
                    if (strlen(name) == 0) {
                        valid = 0;
                    }
                    b_names[i] = strdup(name);
                    b_notes[i] = strdup(note);
                } else {
                    valid = 0;
                    b_names[i] = strdup("");
                    b_notes[i] = strdup("");
                }
                free(bline);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *tmp = realloc(records, new_cap * sizeof(Record));
                        if (!tmp) return 1;
                        records = tmp;
                        capacity = new_cap;
                    }
                    records[count].name = b_names[i];
                    records[count].note = b_notes[i];
                    count++;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    free(b_names[i]);
                    free(b_notes[i]);
                }
            }
            free(b_names);
            free(b_notes);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(records[found].name);
                free(records[found].note);
                for (int i = found; i < count - 1; i++) {
                    records[i] = records[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", records[found].note);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
