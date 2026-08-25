// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

Record *records = NULL;
int num_records = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            sscanf(line, "%*s %s %s", key, val);
            records = realloc(records, (num_records + 1) * sizeof(Record));
            records[num_records].key = strdup(key);
            records[num_records].value = strdup(val);
            num_records++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < num_records; ) {
                int dup = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < num_records - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    num_records--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            sscanf(line, "%*s %s", key);
            for (int i = 0; i < num_records; ) {
                if (strcmp(records[i].key, key) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int j = i; j < num_records - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    num_records--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            sscanf(line, "%*s %s", key);
            for (int i = 0; i < num_records; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < num_records; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }
    
    for (int i = 0; i < num_records; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    
    return 0;
}