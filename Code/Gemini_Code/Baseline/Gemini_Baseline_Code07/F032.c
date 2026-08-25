// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
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

int main() {
    Setting *committed = NULL;
    int c_count = 0;
    int c_capacity = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int s_capacity = 0;
    int in_trans = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            s_count = 0;
            free(line);
            continue;
        }
        if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_trans = 0;
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
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
                        committed[found].value = staged[i].value;
                        free(staged[i].key);
                    } else {
                        if (c_count >= c_capacity) {
                            c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                            committed = realloc(committed, c_capacity * sizeof(Setting));
                        }
                        committed[c_count++] = staged[i];
                    }
                }
                s_count = 0;
                in_trans = 0;
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "SET") == 0) {
            char *key_start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            int key_len = p - key_start;
            char *key = malloc(key_len + 1);
            memcpy(key, key_start, key_len);
            key[key_len] = '\0';
            while (*p == ' ' || *p == '\t') p++;
            int found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(committed[found].value);
                committed[found].value = strdup(p);
                free(key);
            } else {
                if (c_count >= c_capacity) {
                    c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                    committed = realloc(committed, c_capacity * sizeof(Setting));
                }
                committed[c_count].key = key;
                committed[c_count].value = strdup(p);
                count = c_count + 1;
                c_count++;
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                char *key_start = p;
                while (*p && *p != ' ' && *p != '\t') p++;
                int key_len = p - key_start;
                char *key = malloc(key_len + 1);
                memcpy(key, key_start, key_len);
                key[key_len] = '\0';
                while (*p == ' ' || *p == '\t') p++;
                int found = -1;
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(staged[found].value);
                    staged[found].value = strdup(p);
                    free(key);
                } else {
                    if (s_count >= s_capacity) {
                        s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                        staged = realloc(staged, s_capacity * sizeof(Setting));
                    }
                    staged[s_count].key = key;
                    staged[s_count].value = strdup(p);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                int found = -1;
                if (in_trans) {
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        printf("%s\n", staged[found].value);
                    }
                }
                if (found == -1) {
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(committed[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        printf("%s\n", committed[found].value);
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
