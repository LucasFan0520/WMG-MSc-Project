// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} BatchRecord;

int main() {
    BatchRecord *records = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "BATCH") == 0) {
            int n = 0;
            if (sscanf(args, "%d", &n) == 1) {
                char **batch_lines = malloc(n * sizeof(char *));
                for (int i = 0; i < n; i++) {
                    char bline[8192];
                    if (fgets(bline, sizeof(bline), stdin)) {
                        bline[strcspn(bline, "\r\n")] = 0;
                        batch_lines[i] = strdup(bline);
                    } else {
                        batch_lines[i] = strdup("");
                    }
                }
                int batch_valid = 1;
                for (int i = 0; i < n; i++) {
                    char name[256];
                    int b = 0;
                    if (sscanf(batch_lines[i], "%255s%n", name, &b) <= 0) {
                        batch_valid = 0;
                        break;
                    }
                    char *note = batch_lines[i] + b;
                    while (*note == ' ') note++;
                    if (strlen(note) == 0) {
                        batch_valid = 0;
                        break;
                    }
                }
                if (batch_valid) {
                    for (int i = 0; i < n; i++) {
                        char name[256];
                        int b = 0;
                        sscanf(batch_lines[i], "%255s%n", name, &b);
                        char *note = batch_lines[i] + b;
                        while (*note == ' ') note++;
                        records = realloc(records, (count + 1) * sizeof(BatchRecord));
                        records[count].name = strdup(name);
                        records[count].note = strdup(note);
                        count++;
                    }
                }
                for (int i = 0; i < n; i++) {
                    free(batch_lines[i]);
                }
                free(batch_lines);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
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
                records = realloc(records, count * sizeof(BatchRecord));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        printf("%s: %s\n", records[i].name, records[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", records[i].name, records[i].note);
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
