// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main(void) {
    CleaningRecord *records = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            if (*key != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    CleaningRecord *new_records = realloc(records, capacity * sizeof(CleaningRecord));
                    if (new_records) records = new_records;
                }
                if (count < capacity) {
                    records[count].key = strdup(key);
                    records[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            size_t idx = 0;
            while (idx < count) {
                int is_dup = 0;
                for (size_t i = 0; i < idx; i++) {
                    if (strcmp(records[i].key, records[idx].key) == 0) {
                        is_dup = 1;
                        break;
                    }
                }
                if (is_dup) {
                    free(records[idx].key);
                    free(records[idx].value);
                    for (size_t j = idx; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(records[idx].key, key) == 0) {
                    free(records[idx].key);
                    free(records[idx].value);
                    for (size_t j = idx; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].key);
        free(records[i].value);
    }
    free(records);
    return 0;
}
