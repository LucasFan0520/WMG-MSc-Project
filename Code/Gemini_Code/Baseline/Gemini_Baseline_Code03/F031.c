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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "BATCH") == 0) {
            int n = 0;
            if (sscanf(ptr, "%d", &n) > 0 && n > 0) {
                BatchRecord *temp_batch = malloc(n * sizeof(BatchRecord));
                int all_valid = 1;
                for (int i = 0; i < n; i++) {
                    temp_batch[i].name = NULL;
                    temp_batch[i].note = NULL;
                }
                for (int i = 0; i < n; i++) {
                    char bline[4096];
                    if (!fgets(bline, sizeof(bline), stdin)) {
                        all_valid = 0;
                        break;
                    }
                    int blen = strlen(bline);
                    while (blen > 0 && (bline[blen - 1] == '\n' || bline[blen - 1] == '\r')) {
                        bline[blen - 1] = '\0';
                        blen--;
                    }
                    char name[256];
                    int name_len = 0;
                    if (sscanf(bline, "%255s%n", name, &name_len) > 0) {
                        char *note = bline + name_len;
                        while (*note == ' ') note++;
                        temp_batch[i].name = strdup(name);
                        temp_batch[i].note = strdup(note);
                    } else {
                        all_valid = 0;
                    }
                }
                if (all_valid) {
                    records = realloc(records, (count + n) * sizeof(BatchRecord));
                    for (int i = 0; i < n; i++) {
                        records[count + i] = temp_batch[i];
                    }
                    count += n;
                    free(temp_batch);
                } else {
                    for (int i = 0; i < n; i++) {
                        free(temp_batch[i].name);
                        free(temp_batch[i].note);
                    }
                    free(temp_batch);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
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
                    if (count > 0) {
                        records = realloc(records, count * sizeof(BatchRecord));
                    } else {
                        free(records);
                        records = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", records[found].note);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
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
