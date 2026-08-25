// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *key;
    char *value;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *new_r = realloc(records, capacity * sizeof(Record));
                    if (new_r) records = new_r;
                }
                if (count < capacity) {
                    records[count].key = strdup(key);
                    records[count].value = strdup(val);
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
                printf("%s: %s\n", records[i].key, records[i].value);
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
