// F032.c
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
    if (c == EOF && len == 0) {
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
    int c_cap = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int s_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
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
        if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            free(line);
            continue;
        }
        if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = staged[i].value;
                            free(staged[i].key);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 16 : c_cap * 2;
                            Setting *nb = realloc(committed, c_cap * sizeof(Setting));
                            if (nb) committed = nb;
                        }
                        committed[c_count] = staged[i];
                        c_count++;
                    }
                }
                s_count = 0;
                in_tx = 0;
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            int found = 0;
            for (int j = 0; j < c_count; j++) {
                if (strcmp(committed[j].key, key) == 0) {
                    free(committed[j].value);
                    committed[j].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 16 : c_cap * 2;
                    Setting *nb = realloc(committed, c_cap * sizeof(Setting));
                    if (nb) committed = nb;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(value);
                c_count++;
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ') p++;
                char *value = p;
                int found = 0;
                for (int j = 0; j < s_count; j++) {
                    if (strcmp(staged[j].key, key) == 0) {
                        free(staged[j].value);
                        staged[j].value = strdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 16 : s_cap * 2;
                        Setting *nb = realloc(staged, s_cap * sizeof(Setting));
                        if (nb) staged = nb;
                    }
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            if (in_tx) {
                for (int j = 0; j < s_count; j++) {
                    if (strcmp(staged[j].key, key) == 0) {
                        printf("%s\n", staged[j].value);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, key) == 0) {
                        printf("%s\n", committed[j].value);
                        found = 1;
                        break;
                    }
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
