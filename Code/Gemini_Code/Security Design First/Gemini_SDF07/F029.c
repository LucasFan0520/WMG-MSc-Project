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
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *k = NULL;
            char *v = NULL;
            if (space) {
                *space = '\0';
                k = args;
                v = space + 1;
            } else {
                k = args;
                v = "";
            }
            char *n_key = strdup(k);
            char *n_val = strdup(v);
            if (n_key && n_val) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    KVRecord *tmp = realloc(records, new_cap * sizeof(KVRecord));
                    if (tmp) {
                        records = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_key); free(n_val);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                records[count].key = n_key;
                records[count].value = n_val;
                count++;
            } else {
                free(n_key); free(n_val);
            }
            if (space) *space = ' ';
        } else if (strcmp(line, "DEDUP") == 0) {
            int i = 0;
            while (i < count) {
                int is_duplicate = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(records[j].key, records[i].key) == 0) {
                        is_duplicate = 1;
                        break;
                    }
                }
                if (is_duplicate) {
                    free(records[i].key);
                    free(records[i].value);
                    for (int k = i; k < count - 1; k++) {
                        records[k] = records[k + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *k = line + 7;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *k = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].key, k) == 0) {
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
