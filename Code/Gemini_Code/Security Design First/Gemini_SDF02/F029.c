/* F029.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KVRecord;

int main(void) {
    KVRecord *records = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                KVRecord *temp = realloc(records, new_cap * sizeof(KVRecord));
                if (!temp) break;
                records = temp;
                capacity = new_cap;
            }
            records[count].key = strdup(key);
            records[count].value = strdup(val);
            if (records[count].key && records[count].value) count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; ) {
                    if (strcmp(records[i].key, records[j].key) == 0) {
                        free(records[j].key);
                        free(records[j].value);
                        for (size_t k = j; k < count - 1; k++) {
                            records[k] = records[k + 1];
                        }
                        count--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(records[i].key, key) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    printf("%s\n", records[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}
