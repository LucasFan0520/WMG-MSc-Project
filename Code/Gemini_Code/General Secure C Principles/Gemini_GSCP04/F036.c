// F036.c
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
    char *name;
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *records = NULL;
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
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *len_part = p;
            char *colon = strchr(len_part, ':');
            if (colon && *name != '\0') {
                *colon = '\0';
                long long length = atoll(len_part);
                char *data_start = colon + 1;
                if (length >= 0 && strlen(data_start) >= (size_t)length) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        PrefixedRecord *new_records = realloc(records, capacity * sizeof(PrefixedRecord));
                        if (new_records) records = new_records;
                    }
                    if (count < capacity) {
                        records[count].name = strdup(name);
                        records[count].data = malloc(length + 1);
                        if (records[count].data) {
                            memcpy(records[count].data, data_start, length);
                            records[count].data[length] = '\0';
                        }
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *dt = records[i].data;
                    while (*dt) {
                        if (*dt == ' ') putchar('_');
                        else putchar(*dt);
                        dt++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
