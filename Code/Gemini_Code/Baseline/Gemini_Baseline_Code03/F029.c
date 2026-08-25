// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} DupRecord;

int main() {
    DupRecord *records = NULL;
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
        if (strcmp(cmd, "ADD") == 0) {
            char key[256];
            int k_len = 0;
            if (sscanf(ptr, "%255s%n", key, &k_len) > 0) {
                char *val = ptr + k_len;
                while (*val == ' ') val++;
                records = realloc(records, (count + 1) * sizeof(DupRecord));
                records[count].key = strdup(key);
                records[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int i = 0;
            while (i < count) {
                int first_occurrence = -1;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[j].key, records[i].key) == 0) {
                        first_occurrence = j;
                        break;
                    }
                }
                if (first_occurrence != -1) {
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
            if (count > 0) {
                records = realloc(records, count * sizeof(DupRecord));
            } else {
                free(records);
                records = NULL;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int i = 0;
                while (i < count) {
                    if (strcmp(records[i].key, key) == 0) {
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
                if (count > 0) {
                    records = realloc(records, count * sizeof(DupRecord));
                } else {
                    free(records);
                    records = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, key) == 0) {
                        printf("%s\n", records[i].value);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", records[i].key, records[i].value);
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
