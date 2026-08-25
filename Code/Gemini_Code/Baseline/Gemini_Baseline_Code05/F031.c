// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImportRecord;

ImportRecord *records = NULL;
int count = 0;
int capacity = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            char **batch_lines = malloc(n * sizeof(char *));
            int all_valid = 1;
            for (int i = 0; i < n; i++) {
                char bline[4096];
                if (fgets(bline, sizeof(bline), stdin)) {
                    bline[strcspn(bline, "\r\n")] = 0;
                    batch_lines[i] = strdup(bline);
                    char *sp = strchr(batch_lines[i], ' ');
                    if (!sp || sp == batch_lines[i]) {
                        all_valid = 0;
                    }
                } else {
                    all_valid = 0;
                    batch_lines[i] = strdup("");
                }
            }
            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    char *sp = strchr(batch_lines[i], ' ');
                    *sp = 0;
                    char *name = batch_lines[i];
                    char *note = sp + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        records = realloc(records, capacity * sizeof(ImportRecord));
                    }
                    records[count].name = strdup(name);
                    records[count].note = strdup(note);
                    count++;
                }
            }
            for (int i = 0; i < n; i++) {
                free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
