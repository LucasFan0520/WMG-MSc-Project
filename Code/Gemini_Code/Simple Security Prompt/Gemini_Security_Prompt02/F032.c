// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
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
    Setting *com = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    Setting *stg = NULL;
    size_t s_count = 0;
    size_t s_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *key = line + 4;
            char *space = strchr(key, ' ');
            if (space) {
                *space = '\0';
                char *value = space + 1;
                int found = -1;
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(com[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(com[found].value);
                    com[found].value = strdup(value);
                } else {
                    if (c_count >= c_cap) {
                        c_cap = c_cap == 0 ? 4 : c_cap * 2;
                        com = realloc(com, c_cap * sizeof(Setting));
                    }
                    com[c_count].key = strdup(key);
                    com[c_count].value = strdup(value);
                    c_count++;
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(stg[i].key);
                free(stg[i].value);
            }
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *key = line + 5;
                char *space = strchr(key, ' ');
                if (space) {
                    *space = '\0';
                    char *value = space + 1;
                    int found = -1;
                    for (size_t i = 0; i < s_count; i++) {
                        if (strcmp(stg[i].key, key) == 0) {
                            found = (int)i;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(stg[found].value);
                        stg[found].value = strdup(value);
                    } else {
                        if (s_count >= s_cap) {
                            s_cap = s_cap == 0 ? 4 : s_cap * 2;
                            stg = realloc(stg, s_cap * sizeof(Setting));
                        }
                        stg[s_count].key = strdup(key);
                        stg[s_count].value = strdup(value);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = -1;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(com[j].key, stg[i].key) == 0) {
                            found = (int)j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(com[found].value);
                        com[found].value = strdup(stg[i].value);
                    } else {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 4 : c_cap * 2;
                            com = realloc(com, c_cap * sizeof(Setting));
                        }
                        com[c_count].key = strdup(stg[i].key);
                        com[c_count].value = strdup(stg[i].value);
                        c_count++;
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(stg[i].key);
                    free(stg[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(stg[i].key);
                    free(stg[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            char *val = NULL;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(stg[i].key, key) == 0) {
                        val = stg[i].value;
                        break;
                    }
                }
            }
            if (!val) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(com[i].key, key) == 0) {
                        val = com[i].value;
                        break;
                    }
                }
            }
            if (val) printf("%s\n", val);
        } else if (strcmp(line, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", com[i].key, com[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(com[i].key);
        free(com[i].value);
    }
    free(com);
    for (size_t i = 0; i < s_count; i++) {
        free(stg[i].key);
        free(stg[i].value);
    }
    free(stg);
    return 0;
}
