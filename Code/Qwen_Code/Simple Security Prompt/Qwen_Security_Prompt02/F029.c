// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

Record *records = NULL;
int r_count = 0;

int main() {
    char cmd[32];
    char key[1024];
    char value[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", key);
            fgets(value, sizeof(value), stdin);
            if (value[0] == ' ') {
                memmove(value, value + 1, strlen(value));
            }
            value[strcspn(value, "\n")] = '\0';
            records = realloc(records, sizeof(Record) * (r_count + 1));
            records[r_count].key = strdup(key);
            records[r_count].value = strdup(value);
            r_count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < r_count; i++) {
                for (int j = i + 1; j < r_count; ) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        free(records[j].key);
                        free(records[j].value);
                        for (int k = j; k < r_count - 1; k++) {
                            records[k] = records[k + 1];
                        }
                        r_count--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", key);
            for (int i = 0; i < r_count; ) {
                if (strcmp(records[i].key, key) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < r_count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    r_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", key);
            for (int i = 0; i < r_count; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);

    return 0;
}