// F032.c
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
} Setting;

int main(void) {
    Setting *committed = NULL;
    size_t c_count = 0;
    size_t c_capacity = 0;
    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_capacity = 0;
    int in_transaction = 0;
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found && *key != '\0') {
                if (c_count >= c_capacity) {
                    c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                    if (new_c) committed = new_c;
                }
                if (c_count < c_capacity) {
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_transaction) {
                while (*p == ' ' || *p == '\t') p++;
                char *key = p;
                while (*p && *p != ' ' && *p != '\t') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ' || *p == '\t') p++;
                char *val = p;
                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found && *key != '\0') {
                    if (s_count >= s_capacity) {
                        s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *new_s = realloc(staged, s_capacity * sizeof(Setting));
                        if (new_s) staged = new_s;
                    }
                    if (s_count < s_capacity) {
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_transaction) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_capacity) {
                            c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                            if (new_c) committed = new_c;
                        }
                        if (c_count < c_capacity) {
                            committed[c_count].key = strdup(staged[i].key);
                            committed[c_count].value = strdup(staged[i].value);
                            c_count++;
                        }
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_transaction) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    printf("%s\n", committed[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (size_t i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}
