// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} DCRecord;

int main(void) {
    DCRecord *records = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            char *val = key_end;
            if (*key_end != '\0') {
                *key_end = '\0';
                val++;
            }
            while (*val == ' ') val++;
            char *val_end = val;
            while (*val_end && *val_end != ' ') val_end++;
            *val_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                DCRecord *new_r = realloc(records, capacity * sizeof(DCRecord));
                if (new_r) records = new_r;
            }
            records[count].key = mystrdup(key);
            records[count].value = mystrdup(val);
            count++;
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
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
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
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
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
