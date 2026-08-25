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
        if (strcmp(cmd, "ADD") == 0) {
            char k[256], v[256];
            if (sscanf(args, "%255s %255s", k, v) == 2) {
                records = realloc(records, (count + 1) * sizeof(Record));
                records[count].key = strdup(k);
                records[count].value = strdup(v);
                count++;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int i = 0;
            while (i < count) {
                int first = 1;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[j].key, records[i].key) == 0) {
                        first = 0;
                        break;
                    }
                }
                if (!first) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            records = realloc(records, count * sizeof(Record));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char k[256];
            if (sscanf(args, "%255s", k) == 1) {
                int i = 0;
                while (i < count) {
                    if (strcmp(records[i].key, k) == 0) {
                        free(records[i].key);
                        free(records[i].value);
                        for (int j = i; j < count - 1; j++) {
                            records[j] = records[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                records = realloc(records, count * sizeof(Record));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char k[256];
            if (sscanf(args, "%255s", k) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, k) == 0) {
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
