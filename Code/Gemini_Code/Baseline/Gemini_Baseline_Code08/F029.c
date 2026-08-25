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

void add_record(const char *key, const char *value) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        records = realloc(records, capacity * sizeof(Record));
    }
    records[count].key = strdup(key);
    records[count].value = strdup(value);
    count++;
}

void dedup_records(void) {
    int i = 0;
    while (i < count) {
        int duplicate = 0;
        for (int j = 0; j < i; j++) {
            if (strcmp(records[j].key, records[i].key) == 0) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) {
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
}

void delete_key(const char *key) {
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
}

void find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].key, key) == 0) {
            printf("%s\n", records[i].value);
        }
    }
}

void report_records(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", records[i].key, records[i].value);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) add_record(key, val);
        } else if (strcmp(cmd, "DEDUP") == 0) {
            dedup_records();
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = strtok(NULL, "");
            if (key) delete_key(key);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = strtok(NULL, "");
            if (key) find_key(key);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_records();
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
