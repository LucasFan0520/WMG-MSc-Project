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
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main() {
    Setting *com = NULL;
    int com_cnt = 0, com_cap = 0;
    Setting *stg = NULL;
    int stg_cnt = 0, stg_cap = 0;
    int in_tx = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            size_t vl = strlen(value);
            while (vl > 0 && (value[vl-1] == '\r' || value[vl-1] == '\n')) {
                value[vl-1] = '\0';
                vl--;
            }
            int found = 0;
            for (int i = 0; i < com_cnt; i++) {
                if (strcmp(com[i].key, key) == 0) {
                    free(com[i].value);
                    com[i].value = mystrdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (com_cnt >= com_cap) {
                    com_cap = com_cap == 0 ? 8 : com_cap * 2;
                    Setting *ncom = realloc(com, com_cap * sizeof(Setting));
                    if (ncom) com = ncom;
                }
                if (com_cnt < com_cap) {
                    com[com_cnt].key = mystrdup(key);
                    com[com_cnt].value = mystrdup(value);
                    com_cnt++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < stg_cnt; i++) {
                free(stg[i].key);
                free(stg[i].value);
            }
            stg_cnt = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                while (*p == ' ' || *p == '\t') p++;
                char *key = p;
                while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ' || *p == '\t') p++;
                char *value = p;
                size_t vl = strlen(value);
                while (vl > 0 && (value[vl-1] == '\r' || value[vl-1] == '\n')) {
                    value[vl-1] = '\0';
                    vl--;
                }
                int found = 0;
                for (int i = 0; i < stg_cnt; i++) {
                    if (strcmp(stg[i].key, key) == 0) {
                        free(stg[i].value);
                        stg[i].value = mystrdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (stg_cnt >= stg_cap) {
                        stg_cap = stg_cap == 0 ? 8 : stg_cap * 2;
                        Setting *nstg = realloc(stg, stg_cap * sizeof(Setting));
                        if (nstg) stg = nstg;
                    }
                    if (stg_cnt < stg_cap) {
                        stg[stg_cnt].key = mystrdup(key);
                        stg[stg_cnt].value = mystrdup(value);
                        stg_cnt++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < stg_cnt; i++) {
                    int found = 0;
                    for (int j = 0; j < com_cnt; j++) {
                        if (strcmp(com[j].key, stg[i].key) == 0) {
                            free(com[j].value);
                            com[j].value = mystrdup(stg[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (com_cnt >= com_cap) {
                            com_cap = com_cap == 0 ? 8 : com_cap * 2;
                            Setting *ncom = realloc(com, com_cap * sizeof(Setting));
                            if (ncom) com = ncom;
                        }
                        if (com_cnt < com_cap) {
                            com[com_cnt].key = mystrdup(stg[i].key);
                            com[com_cnt].value = mystrdup(stg[i].value);
                            com_cnt++;
                        }
                    }
                }
                for (int i = 0; i < stg_cnt; i++) {
                    free(stg[i].key);
                    free(stg[i].value);
                }
                stg_cnt = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < stg_cnt; i++) {
                    free(stg[i].key);
                    free(stg[i].value);
                }
                stg_cnt = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int printed = 0;
            if (in_tx) {
                for (int i = 0; i < stg_cnt; i++) {
                    if (strcmp(stg[i].key, key) == 0) {
                        printf("%s\n", stg[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
            if (!printed) {
                for (int i = 0; i < com_cnt; i++) {
                    if (strcmp(com[i].key, key) == 0) {
                        printf("%s\n", com[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
            if (!printed) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < com_cnt; i++) {
                printf("%s %s\n", com[i].key, com[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < com_cnt; i++) {
        free(com[i].key);
        free(com[i].value);
    }
    free(com);
    for (int i = 0; i < stg_cnt; i++) {
        free(stg[i].key);
        free(stg[i].value);
    }
    free(stg);
    return 0;
}
