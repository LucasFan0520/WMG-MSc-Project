// F009.c
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
    char *name;
    char **evs;
    int count;
    int cap;
} Group;

int main() {
    Group *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            char *gname = get_token(&p);
            char *desc = get_rest(&p);
            if (gname) {
                int gidx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, gname) == 0) {
                        gidx = i;
                        break;
                    }
                }
                if (gidx == -1) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Group *narr = realloc(arr, cap * sizeof(Group));
                        if (narr) arr = narr;
                    }
                    arr[count].name = strdup(gname);
                    arr[count].evs = NULL;
                    arr[count].count = 0;
                    arr[count].cap = 0;
                    gidx = count;
                    count++;
                }
                Group *g = &arr[gidx];
                if (g->count >= g->cap) {
                    g->cap = g->cap == 0 ? 4 : g->cap * 2;
                    char **nevs = realloc(g->evs, g->cap * sizeof(char *));
                    if (nevs) g->evs = nevs;
                }
                g->evs[g->count] = strdup(desc);
                g->count++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = get_token(&p);
            char *gB = get_token(&p);
            char *gN = get_token(&p);
            if (gA && gB && gN) {
                int idxA = -1, idxB = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, gA) == 0) idxA = i;
                    if (strcmp(arr[i].name, gB) == 0) idxB = i;
                }
                int ncap = 4;
                int ncnt = 0;
                char **nevs = malloc(ncap * sizeof(char *));
                if (idxA != -1) {
                    for (int i = 0; i < arr[idxA].count; i++) {
                        if (ncnt >= ncap) {
                            ncap *= 2;
                            nevs = realloc(nevs, ncap * sizeof(char *));
                        }
                        nevs[ncnt++] = strdup(arr[idxA].evs[i]);
                    }
                }
                if (idxB != -1) {
                    for (int i = 0; i < arr[idxB].count; i++) {
                        if (ncnt >= ncap) {
                            ncap *= 2;
                            nevs = realloc(nevs, ncap * sizeof(char *));
                        }
                        nevs[ncnt++] = strdup(arr[idxB].evs[i]);
                    }
                }
                int idxN = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, gN) == 0) {
                        idxN = i;
                        break;
                    }
                }
                if (idxN != -1) {
                    for (int i = 0; i < arr[idxN].count; i++) {
                        free(arr[idxN].evs[i]);
                    }
                    free(arr[idxN].evs);
                    arr[idxN].evs = nevs;
                    arr[idxN].count = ncnt;
                    arr[idxN].cap = ncap;
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Group *narr = realloc(arr, cap * sizeof(Group));
                        if (narr) arr = narr;
                    }
                    arr[count].name = strdup(gN);
                    arr[count].evs = nevs;
                    arr[count].count = ncnt;
                    arr[count].cap = ncap;
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *gname = get_token(&p);
            if (gname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, gname) == 0) {
                        free(arr[i].name);
                        for (int j = 0; j < arr[i].count; j++) {
                            free(arr[i].evs[j]);
                        }
                        free(arr[i].evs);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *gname = get_token(&p);
            if (gname) {
                int cnt = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, gname) == 0) {
                        cnt = arr[i].count;
                        break;
                    }
                }
                printf("%d\n", cnt);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        for (int j = 0; j < arr[i].count; j++) {
            free(arr[i].evs[j]);
        }
        free(arr[i].evs);
    }
    free(arr);
    return 0;
}
