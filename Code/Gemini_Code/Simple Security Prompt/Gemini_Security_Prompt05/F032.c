// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main() {
    Setting *comm = NULL;
    int c_count = 0;
    int c_cap = 0;
    Setting *stag = NULL;
    int s_count = 0;
    int s_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = get_token(&p);
            char *val = get_rest(&p);
            if (key) {
                int found = 0;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(comm[i].key, key) == 0) {
                        free(comm[i].value);
                        comm[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (c_count >= c_cap) {
                        c_cap = c_cap == 0 ? 4 : c_cap * 2;
                        Setting *ncomm = realloc(comm, c_cap * sizeof(Setting));
                        if (ncomm) comm = ncomm;
                    }
                    comm[c_count].key = strdup(key);
                    comm[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < s_count; i++) {
                free(stag[i].key);
                free(stag[i].value);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *key = get_token(&p);
                char *val = get_rest(&p);
                if (key) {
                    int found = 0;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(stag[i].key, key) == 0) {
                            free(stag[i].value);
                            stag[i].value = strdup(val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (s_count >= s_cap) {
                            s_cap = s_cap == 0 ? 4 : s_cap * 2;
                            Setting *nstag = realloc(stag, s_cap * sizeof(Setting));
                            if (nstag) stag = nstag;
                        }
                        stag[s_count].key = strdup(key);
                        stag[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(comm[j].key, stag[i].key) == 0) {
                            free(comm[j].value);
                            comm[j].value = strdup(stag[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_cap) {
                            c_cap = c_cap == 0 ? 4 : c_cap * 2;
                            Setting *ncomm = realloc(comm, c_cap * sizeof(Setting));
                            if (ncomm) comm = ncomm;
                        }
                        comm[c_count].key = strdup(stag[i].key);
                        comm[c_count].value = strdup(stag[i].value);
                        c_count++;
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(stag[i].key);
                    free(stag[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    free(stag[i].key);
                    free(stag[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = get_token(&p);
            if (key) {
                int found = 0;
                if (in_tx) {
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(stag[i].key, key) == 0) {
                            printf("%s\n", stag[i].value);
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) {
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(comm[i].key, key) == 0) {
                            printf("%s\n", comm[i].value);
                            found = 1;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s %s\n", comm[i].key, comm[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(comm[i].key);
        free(comm[i].value);
    }
    free(comm);
    for (int i = 0; i < s_count; i++) {
        free(stag[i].key);
        free(stag[i].value);
    }
    free(stag);
    return 0;
}
