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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                while (*val == ' ') val++;
                int found = -1;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(committed[found].value);
                    committed[found].value = strdup(val);
                } else {
                    if (c_count >= c_cap) {
                        c_cap = c_cap == 0 ? 16 : c_cap * 2;
                        committed = realloc(committed, c_cap * sizeof(Setting));
                    }
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 1;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *p = line + 5;
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *val = p + 1;
                    while (*val == ' ') val++;
                    int found = -1;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(staged[found].value);
                        staged[found].value = strdup(val);
                    } else {
                        if (s_count >= s_cap) {
                            s_cap = s_cap == 0 ? 16 : s_cap * 2;
                            staged = realloc(staged, s_cap * sizeof(Setting));
                        }
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
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
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 16 : c_cap * 2;
                            committed = realloc(committed, c_cap * sizeof(Setting));
                        }
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            int found = -1;
            if (in_tx) {
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
