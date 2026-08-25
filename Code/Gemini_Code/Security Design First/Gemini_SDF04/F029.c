// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

Record *records = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *val = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Record *tmp = realloc(records, capacity * sizeof(Record));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        records = tmp;
                    }
                    records[count].key = strdup(arg1);
                    records[count].value = strdup(val);
                    if (records[count].key && records[count].value) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(records[i].key, arg1) == 0) {
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
            } else if (strcmp(cmd, "FIND") == 0) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, arg1) == 0) {
                        printf("%s\n", records[i].value);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "DEDUP") == 0) {
                for (int i = 0; i < count; i++) {
                    for (int j = i + 1; j < count; ) {
                        if (strcmp(records[i].key, records[j].key) == 0) {
                            free(records[j].key);
                            free(records[j].value);
                            for (int k = j; k < count - 1; k++) {
                                records[k] = records[k + 1];
                            }
                            count--;
                        } else {
                            j++;
                        }
                    }
                }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", records[i].key, records[i].value);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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