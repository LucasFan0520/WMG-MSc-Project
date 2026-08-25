// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *val;
} Setting;

int main() {
    Setting *committed = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *key = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *value = p;
        if (strcmp(cmd, "SET") == 0) {
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].val);
                    committed[i].val = mystrdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 4 : c_cap * 2;
                    committed = realloc(committed, c_cap * sizeof(Setting));
                }
                committed[c_count].key = mystrdup(key);
                committed[c_count].val = mystrdup(value);
                c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].val);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].val);
                        staged[i].val = mystrdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 4 : s_cap * 2;
                        staged = realloc(staged, s_cap * sizeof(Setting));
                    }
                    staged[s_count].key = mystrdup(key);
                    staged[s_count].val = mystrdup(value);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].val);
                            committed[j].val = mystrdup(staged[i].val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 4 : c_cap * 2;
                            committed = realloc(committed, c_cap * sizeof(Setting));
                        }
                        committed[c_count].key = mystrdup(staged[i].key);
                        committed[c_count].val = mystrdup(staged[i].val);
                        c_count++;
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].val);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].val);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            int found = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        printf("%s\n", staged[i].val);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        printf("%s\n", committed[i].val);
                        found = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].val);
    }
    free(committed);
    for (size_t i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].val);
    }
    free(staged);
    return 0;
}
