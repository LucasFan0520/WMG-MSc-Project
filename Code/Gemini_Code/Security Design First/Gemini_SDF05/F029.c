// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <span style="font-family:monospace">string.h</span>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            char *val = "";
            if (space) {
                *space = '\0';
                val = space + 1;
                while (*val == ' ') val++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *tmp = realloc(records, new_cap * sizeof(Record));
                if (!tmp) return 1;
                records = tmp;
                capacity = new_cap;
            }
            records[count].key = strdup(key);
            records[count].value = strdup(val);
            count++;
        } else if (strcmp(line, "DEDUP") == 0) {
            int w = 0;
            for (int i = 0; i < count; i++) {
                int dup = 0;
                for (int j = 0; j < w; j++) {
                    if (strcmp(records[j].key, records[i].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(records[i].key);
                    free(records[i].value);
                } else {
                    records[w++] = records[i];
                }
            }
            count = w;
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
            int w = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].key, key) == 0) {
                    free(records[i].key);
                    free(records[i].value);
                } else {
                    records[w++] = records[i];
                }
            }
            count = w;
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
