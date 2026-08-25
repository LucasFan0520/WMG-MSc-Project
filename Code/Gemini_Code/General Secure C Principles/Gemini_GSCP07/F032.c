// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Setting *committed = NULL;
    size_t c_count = 0;
    size_t c_capacity = 0;
    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_capacity = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
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
            if (!found) {
                if (c_count >= c_capacity) {
                    c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *nc = realloc(committed, c_capacity * sizeof(Setting));
                    if (!nc) break;
                    committed = nc;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(val);
                c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ') p++;
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
                if (!found) {
                    if (s_count >= s_capacity) {
                        s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *ns = realloc(staged, s_capacity * sizeof(Setting));
                        if (!ns) break;
                        staged = ns;
                    }
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(val);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
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
                            Setting *nc = realloc(committed, c_capacity * sizeof(Setting));
                            if (!nc) break;
                            committed = nc;
                        }
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            char *res = NULL;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        res = staged[i].value;
                        break;
                    }
                }
            }
            if (!res) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        res = committed[i].value;
                        break;
                    }
                }
            }
            if (res) printf("%s\n", res);
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
