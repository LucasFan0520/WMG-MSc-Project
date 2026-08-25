// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *records = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *len_str = p;
            char *colon = strchr(len_str, ':');
            if (colon) {
                *colon = '\0';
                long long expected_len = strtoll(len_str, NULL, 10);
                char *data_start = colon + 1;
                if (expected_len >= 0 && (size_t)expected_len <= strlen(data_start)) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        PrefixedRecord *nb = realloc(records, cap * sizeof(PrefixedRecord));
                        if (!nb) {
                            free(line);
                            break;
                        }
                        records = nb;
                    }
                    records[count].name = safe_dup(name);
                    char *saved_data = malloc(expected_len + 1);
                    if (saved_data) {
                        memcpy(saved_data, data_start, expected_len);
                        saved_data[expected_len] = '\0';
                        records[count].data = saved_data;
                        count++;
                    } else {
                        free(records[count].name);
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *t = records[i].data;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
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
