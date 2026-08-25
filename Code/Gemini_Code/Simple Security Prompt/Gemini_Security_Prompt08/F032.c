// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *committed = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_cap = 0;
    int in_trans = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "SET") == 0) {
            char *key = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 4 : c_cap * 2;
                    Setting *nb = realloc(committed, c_cap * sizeof(Setting));
                    if (nb) committed = nb;
                }
                if (c_count < c_cap) {
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(value);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                char *key = arg1;
                p = arg1;
                while (*p && *p != ' ') p++;
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *value = p;
                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 4 : s_cap * 2;
                        Setting *nb = realloc(staged, s_cap * sizeof(Setting));
                        if (nb) staged = nb;
                    }
                    if (s_count < s_cap) {
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(value);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = staged[i].value;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 4 : c_cap * 2;
                            Setting *nb = realloc(committed, c_cap * sizeof(Setting));
                            if (nb) committed = nb;
                        }
                        if (c_count < c_cap) {
                            committed[c_count].key = strdup(staged[i].key);
                            committed[c_count].value = staged[i].value;
                            c_count++;
                        }
                    } else {
                    }
                    free(staged[i].key);
                }
                s_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            if (in_trans) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, arg1) == 0) {
                        printf("%s\n", staged[i].value);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, arg1) == 0) {
                        printf("%s\n", committed[i].value);
                        found = 1;
                        break;
                    }
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
