// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int main(void) {
    Setting *committed = NULL;
    int com_count = 0;
    Setting *staged = NULL;
    int stag_count = 0;
    int in_trans = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            int found = 0;
            for (int i = 0; i < com_count; i++) {
                if (committed[i].key && strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                committed = realloc(committed, (com_count + 1) * sizeof(Setting));
                committed[com_count].key = strdup(key);
                committed[com_count].value = strdup(val);
                com_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *val = p;
                int found = 0;
                for (int i = 0; i < stag_count; i++) {
                    if (staged[i].key && strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    staged = realloc(staged, (stag_count + 1) * sizeof(Setting));
                    staged[stag_count].key = strdup(key);
                    staged[stag_count].value = strdup(val);
                    stag_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (int i = 0; i < stag_count; i++) {
                    if (staged[i].key) {
                        int found = 0;
                        for (int j = 0; j < com_count; j++) {
                            if (committed[j].key && strcmp(committed[j].key, staged[i].key) == 0) {
                                free(committed[j].value);
                                committed[j].value = staged[i].value;
                                free(staged[i].key);
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            committed = realloc(committed, (com_count + 1) * sizeof(Setting));
                            committed[com_count] = staged[i];
                            com_count++;
                        }
                    }
                }
                free(staged);
                staged = NULL;
                stag_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (int i = 0; i < stag_count; i++) {
                    if (staged[i].key) {
                        free(staged[i].key);
                        free(staged[i].value);
                    }
                }
                free(staged);
                staged = NULL;
                stag_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            char *res = NULL;
            if (in_trans) {
                for (int i = 0; i < stag_count; i++) {
                    if (staged[i].key && strcmp(staged[i].key, key) == 0) {
                        res = staged[i].value;
                        break;
                    }
                }
            }
            if (!res) {
                for (int i = 0; i < com_count; i++) {
                    if (committed[i].key && strcmp(committed[i].key, key) == 0) {
                        res = committed[i].value;
                        break;
                    }
                }
            }
            if (res) {
                printf("%s\n", res);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < com_count; i++) {
                if (committed[i].key) {
                    printf("%s=%s\n", committed[i].key, committed[i].value);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < com_count; i++) {
        if (committed[i].key) {
            free(committed[i].key);
            free(committed[i].value);
        }
    }
    free(committed);
    for (int i = 0; i < stag_count; i++) {
        if (staged[i].key) {
            free(staged[i].key);
            free(staged[i].value);
        }
    }
    free(staged);
    return 0;
}
