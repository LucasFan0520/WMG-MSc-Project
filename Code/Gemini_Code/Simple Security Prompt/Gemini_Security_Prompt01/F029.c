// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main() {
    CleaningRecord *records = NULL;
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

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            char *val = "";
            if (space2) {
                *space2 = '\0';
                val = space2 + 1;
                while (*val == ' ') val++;
            }
            records = realloc(records, (count + 1) * sizeof(CleaningRecord));
            if (!records) return 1;
            records[count].key = strdup(key);
            records[count].value = strdup(val);
            count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int idx = 0;
            while (idx < count) {
                int is_dup = 0;
                for (int i = 0; i < idx; i++) {
                    if (strcmp(records[i].key, records[idx].key) == 0) {
                        is_dup = 1;
                        break;
                    }
                }
                if (is_dup) {
                    free(records[idx].key);
                    free(records[idx].value);
                    for (int j = idx; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            records = realloc(records, count * sizeof(CleaningRecord));
            if (count > 0 && !records) return 1;
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            int idx = 0;
            while (idx < count) {
                if (strcmp(records[idx].key, args) == 0) {
                    free(records[idx].key);
                    free(records[idx].value);
                    for (int j = idx; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
            records = realloc(records, count * sizeof(CleaningRecord));
            if (count > 0 && !records) return 1;
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].key, args) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}
