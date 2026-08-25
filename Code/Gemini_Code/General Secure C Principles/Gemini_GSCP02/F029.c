// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main(void) {
    CleaningRecord *records = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char k[256], v[256];
            if (sscanf(line + 4, "%255s %255s", k, v) == 2) {
                CleaningRecord *tmp = realloc(records, (count + 1) * sizeof(CleaningRecord));
                if (tmp) {
                    records = tmp;
                    records[count].key = strdup(k);
                    records[count].value = strdup(v);
                    count++;
                }
            }
        } else if (strcmp(line, "DEDUP") == 0) {
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *key = line + 5;
            while (*key == ' ') key++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
