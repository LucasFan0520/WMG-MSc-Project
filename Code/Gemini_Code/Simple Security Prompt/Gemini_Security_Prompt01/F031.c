// F031.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImporterRecord;

int main() {
    ImporterRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "BATCH") == 0 && args) {
            int n = atoi(args);
            char **batch_lines = malloc(n * sizeof(char *));
            if (n > 0 && !batch_lines) return 1;
            int all_valid = 1;

            for (int i = 0; i < n; i++) {
                char *bline = NULL;
                size_t blen = 0;
                if (getline(&bline, &blen, stdin) != -1) {
                    bline[strcspn(bline, "\r\n")] = '\0';
                    batch_lines[i] = bline;
                    char *sp = strchr(bline, ' ');
                    if (!sp || sp == bline) {
                        all_valid = 0;
                    }
                } else {
                    all_valid = 0;
                    batch_lines[i] = NULL;
                }
            }

            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    char *bline = batch_lines[i];
                    char *sp = strchr(bline, ' ');
                    *sp = '\0';
                    char *name = bline;
                    char *note = sp + 1;
                    while (*note == ' ') note++;

                    records = realloc(records, (count + 1) * sizeof(ImporterRecord));
                    if (!records) return 1;
                    records[count].name = strdup(name);
                    records[count].note = strdup(note);
                    count++;
                }
            }

            for (int i = 0; i < n; i++) {
                free(batch_lines[i]);
            }
            free(batch_lines);

        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, args) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    records = realloc(records, count * sizeof(ImporterRecord));
                    if (count > 0 && !records) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, args) == 0) {
                    printf("%s %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
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
