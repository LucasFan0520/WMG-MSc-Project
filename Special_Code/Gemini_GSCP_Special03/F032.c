// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
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
    int in_tx = 0;
    while (1) {
        char *line = read_line();
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
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *val = p;
            size_t vlen = strlen(val);
            while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                val[vlen-1] = '\0';
                vlen--;
            }
            int found = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(committed[found].value);
                committed[found].value = mystrdup(val);
            } else {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 8 : c_cap * 2;
                    Setting *nc = realloc(committed, c_cap * sizeof(Setting));
                    if (nc) committed = nc;
                }
                committed[c_count].key = mystrdup(key);
                committed[c_count].value = mystrdup(val);
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
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                    while (*p == ' ') p++;
                }
                char *val = p;
                size_t vlen = strlen(val);
                while (vlen > 0 && (val[vlen-1] == '\n' || val[vlen-1] == '\n')) {
                    val[vlen-1] = '\0';
                    vlen--;
                }
                int found = -1;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(staged[found].value);
                    staged[found].value = mystrdup(val);
                } else {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 8 : s_cap * 2;
                        Setting *ns = realloc(staged, s_cap * sizeof(Setting));
                        if (ns) staged = ns;
                    }
                    staged[s_count].key = mystrdup(key);
                    staged[s_count].value = mystrdup(val);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = -1;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = (int)j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(committed[found].value);
                        committed[found].value = staged[i].value;
                    } else {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 8 : c_cap * 2;
                            Setting *nc = realloc(committed, c_cap * sizeof(Setting));
                            if (nc) committed = nc;
                        }
                        committed[c_count].key = staged[i].key;
                        committed[c_count].value = staged[i].value;
                        c_count++;
                    }
                    staged[i].key = NULL;
                    staged[i].value = NULL;
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
            char *key = p;
            size_t klen = strlen(key);
            while (klen > 0 && (key[klen-1] == '\n' || key[klen-1] == '\n')) {
                key[klen-1] = '\0';
                klen--;
            }
            char *val = NULL;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        val = staged[i].value;
                        break;
                    }
                }
            }
            if (!val) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        val = committed[i].value;
                        break;
                    }
                }
            }
            if (val) {
                printf("%s\n", val);
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
