// F031.c
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
    char *note;
} ImporterRecord;

int main(void) {
    ImporterRecord *records = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long n = atoll(n_s);
            if (n > 0) {
                char **b_lines = malloc(n * sizeof(char *));
                int batch_valid = 1;
                for (long long i = 0; i < n; i++) {
                    b_lines[i] = read_line(stdin);
                    if (!b_lines[i]) {
                        batch_valid = 0;
                    }
                }
                if (batch_valid) {
                    for (long long i = 0; i < n; i++) {
                        char *bp = b_lines[i];
                        while (*bp == ' ' || *bp == '\t') bp++;
                        char *name = bp;
                        while (*bp && *bp != ' ' && *bp != '\t') bp++;
                        if (*bp) {
                            *bp = '\0';
                            bp++;
                        }
                        while (*bp == ' ' || *bp == '\t') bp++;
                        if (*name == '\0') {
                            batch_valid = 0;
                            break;
                        }
                    }
                }
                if (batch_valid) {
                    for (long long i = 0; i < n; i++) {
                        char *bp = b_lines[i];
                        while (*bp == ' ' || *bp == '\t') bp++;
                        char *name = bp;
                        while (*bp && *bp != ' ' && *bp != '\t') bp++;
                        if (*bp) bp++;
                        while (*bp == ' ' || *bp == '\t') bp++;
                        char *note = bp;
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            ImporterRecord *new_records = realloc(records, capacity * sizeof(ImporterRecord));
                            if (new_records) records = new_records;
                        }
                        if (count < capacity) {
                            records[count].name = strdup(name);
                            records[count].note = strdup(note);
                            count++;
                        }
                    }
                }
                for (long long i = 0; i < n; i++) {
                    if (b_lines[i]) free(b_lines[i]);
                }
                free(b_lines);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(records[idx].name, name) == 0) {
                    free(records[idx].name);
                    free(records[idx].note);
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
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
