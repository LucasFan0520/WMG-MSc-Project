// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Setting *committed = NULL;
    int c_count = 0;
    Setting *staged = NULL;
    int s_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "BEGIN") == 0) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            } else if (strcmp(line, "COMMIT") == 0) {
                for (int i = 0; i < s_count; i++) {
                    int found = -1;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(committed[found].value);
                        committed[found].value = strdup(staged[i].value);
                    } else {
                        committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            } else if (strcmp(line, "ABORT") == 0) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            } else if (strcmp(line, "DUMP") == 0) {
                for (int i = 0; i < c_count; i++) {
                    printf("%s=%s\n", committed[i].key, committed[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *value = p2 + 1;
                int found = -1;
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, key) == 0) {
                        found = j;
                        break;
                    }
                }
                if (found != -1) {
                    free(committed[found].value);
                    committed[found].value = strdup(value);
                } else {
                    committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(value);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *value = p2 + 1;
                int found = -1;
                for (int j = 0; j < s_count; j++) {
                    if (strcmp(staged[j].key, key) == 0) {
                        found = j;
                        break;
                    }
                }
                if (found != -1) {
                    free(staged[found].value);
                    staged[found].value = strdup(value);
                } else {
                    staged = realloc(staged, (s_count + 1) * sizeof(Setting));
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = rest;
            int found = -1;
            for (int j = 0; j < s_count; j++) {
                if (strcmp(staged[j].key, key) == 0) {
                    found = j;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", staged[found].value);
            } else {
                int c_found = -1;
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, key) == 0) {
                        c_found = j;
                        break;
                    }
                }
                if (c_found != -1) {
                    printf("%s\n", committed[c_found].value);
                }
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