/* F032.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
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
    char *val;
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
        if (*p == ' ') {
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
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            int found = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nv = mystrdup(val);
                if (nv) {
                    free(committed[found].val);
                    committed[found].val = nv;
                }
            } else {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 4 : c_cap * 2;
                    Setting *nc = realloc(committed, c_cap * sizeof(Setting));
                    if (nc) committed = nc;
                }
                char *nk = mystrdup(key);
                char *nv = mystrdup(val);
                if (nk && nv) {
                    committed[c_count].key = nk;
                    committed[c_count].val = nv;
                    c_count++;
                } else {
                    free(nk); free(nv);
                }
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
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ') p++;
                char *val = p;
                int found = -1;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nv = mystrdup(val);
                    if (nv) {
                        free(staged[found].val);
                        staged[found].val = nv;
                    }
                } else {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 4 : s_cap * 2;
                        Setting *ns = realloc(staged, s_cap * sizeof(Setting));
                        if (ns) staged = ns;
                    }
                    char *nk = mystrdup(key);
                    char *nv = mystrdup(val);
                    if (nk && nv) {
                        staged[s_count].key = nk;
                        staged[s_count].val = nv;
                        s_count++;
                    } else {
                        free(nk); free(nv);
                    }
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
                        char *nv = mystrdup(staged[i].val);
                        if (nv) {
                            free(committed[found].val);
                            committed[found].val = nv;
                        }
                    } else {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 4 : c_cap * 2;
                            Setting *nc = realloc(committed, c_cap * sizeof(Setting));
                            if (nc) committed = nc;
                        }
                        char *nk = mystrdup(staged[i].key);
                        char *nv = mystrdup(staged[i].val);
                        if (nk && nv) {
                            committed[c_count].key = nk;
                            committed[c_count].val = nv;
                            c_count++;
                        } else {
                            free(nk); free(nv);
                        }
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
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", staged[found].val);
                }
            }
            if (found == -1) {
                for (size_t j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, key) == 0) {
                        found = (int)j;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", committed[found].val);
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t j = 0; j < c_count; j++) {
                printf("%s=%s\n", committed[j].key, committed[j].val);
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
