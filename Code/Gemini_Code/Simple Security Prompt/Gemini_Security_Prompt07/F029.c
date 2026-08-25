// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

int main() {
    Record *records = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) {
                records = realloc(records, (count + 1) * sizeof(Record));
                records[count].key = strdup(key);
                records[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        free(records[j].key);
                        free(records[j].value);
                        for (int k = j; k < count - 1; k++) {
                            records[k] = records[k + 1];
                        }
                        count--;
                        j--;
                    }
                }
            }
            if (count == 0) {
                free(records);
                records = NULL;
            } else {
                records = realloc(records, count * sizeof(Record));
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, key) == 0) {
                        free(records[i].key);
                        free(records[i].value);
                        for (int j = i; j < count - 1; j++) {
                            records[j] = records[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(records);
                    records = NULL;
                } else {
                    records = realloc(records, count * sizeof(Record));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, key) == 0) {
                        printf("%s\n", records[i].value);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}
