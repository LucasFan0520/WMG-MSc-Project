// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} PairRecord;

int main() {
    PairRecord *records = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].key, records[i].value);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "DEDUP") == 0) {
            int write_idx = 0;
            for (int i = 0; i < count; i++) {
                int first = 1;
                for (int j = 0; j < write_idx; j++) {
                    if (strcmp(records[j].key, records[i].key) == 0) {
                        first = 0;
                        break;
                    }
                }
                if (first) {
                    records[write_idx++] = records[i];
                } else {
                    free(records[i].key);
                    free(records[i].value);
                }
            }
            count = write_idx;
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            if (sscanf(p, "%255s %255s", key, val) == 2) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    records = realloc(records, capacity * sizeof(PairRecord));
                }
                records[count].key = strdup(key);
                records[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                int write_idx = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, key) == 0) {
                        free(records[i].key);
                        free(records[i].value);
                    } else {
                        records[write_idx++] = records[i];
                    }
                }
                count = write_idx;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].key, key) == 0) {
                        printf("%s\n", records[i].value);
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}
