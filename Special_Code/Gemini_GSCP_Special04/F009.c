/* F009.c */
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
    char *name;
    char **events;
    size_t e_count;
    size_t e_cap;
} Group;

int main(void) {
    Group *groups = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (strcmp(cmd, "EVENT") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *desc = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Group *ng = realloc(groups, cap * sizeof(Group));
                    if (ng) groups = ng;
                }
                char *nn = mystrdup(gname);
                if (nn) {
                    groups[count].name = nn;
                    groups[count].events = NULL;
                    groups[count].e_count = 0;
                    groups[count].e_cap = 0;
                    found = (int)count;
                    count++;
                }
            }
            if (found != -1) {
                Group *g = &groups[found];
                if (g->e_count >= g->e_cap) {
                    g->e_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                    char **ne = realloc(g->events, g->e_cap * sizeof(char *));
                    if (ne) g->events = ne;
                }
                char *nd = mystrdup(desc);
                if (nd) {
                    g->events[g->e_count] = nd;
                    g->e_count++;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*p == ' ') p++;
            char *ga = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *gb = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *gnew = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int idxA = -1, idxB = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, ga) == 0) idxA = (int)i;
                if (strcmp(groups[i].name, gb) == 0) idxB = (int)i;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Group *ng = realloc(groups, cap * sizeof(Group));
                if (ng) groups = ng;
            }
            char *nn = mystrdup(gnew);
            if (nn) {
                groups[count].name = nn;
                groups[count].events = NULL;
                groups[count].e_count = 0;
                groups[count].e_cap = 0;
                Group *g = &groups[count];
                if (idxA != -1) {
                    for (size_t i = 0; i < groups[idxA].e_count; i++) {
                        if (g->e_count >= g->e_cap) {
                            g->e_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                            char **ne = realloc(g->events, g->e_cap * sizeof(char *));
                            if (ne) g->events = ne;
                        }
                        char *nd = mystrdup(groups[idxA].events[i]);
                        if (nd) g->events[g->e_count++] = nd;
                    }
                }
                if (idxB != -1) {
                    for (size_t i = 0; i < groups[idxB].e_count; i++) {
                        if (g->e_count >= g->e_cap) {
                            g->e_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                            char **ne = realloc(g->events, g->e_cap * sizeof(char *));
                            if (ne) g->events = ne;
                        }
                        char *nd = mystrdup(groups[idxB].events[i]);
                        if (nd) g->events[g->e_count++] = nd;
                    }
                }
                count++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(groups[found].name);
                for (size_t j = 0; j < groups[found].e_count; j++) {
                    free(groups[found].events[j]);
                }
                free(groups[found].events);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            size_t cnt = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    cnt = groups[i].e_count;
                    break;
                }
            }
            printf("%zu\n", cnt);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", groups[i].name, groups[i].e_count);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(groups[i].name);
        for (size_t j = 0; j < groups[i].e_count; j++) {
            free(groups[i].events[j]);
        }
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
