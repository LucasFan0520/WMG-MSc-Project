// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *key;
    char *value;
} Setting;

int main() {
    Setting *committed = NULL;
    int c_count = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int in_trans = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                int found = 0;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        free(committed[i].value);
                        committed[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_trans = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_trans) {
                char *p = line + 5;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *val = p + 1;
                    int found = 0;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            free(staged[i].value);
                            staged[i].value = strdup(val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        staged = realloc(staged, (s_count + 1) * sizeof(Setting));
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_trans) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                in_trans = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_trans) {
                in_trans = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            int found = 0;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    printf("%s\n", committed[i].value);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}
