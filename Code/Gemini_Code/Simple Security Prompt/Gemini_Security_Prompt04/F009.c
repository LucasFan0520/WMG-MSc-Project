// F009.c
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
    char *name;
    char **events;
    int count;
    int cap;
} Group;

int main(void) {
    Group *groups = NULL;
    int gcount = 0;
    int gcap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "EVENT ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *gname = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *desc = p + 1;
                while (*desc == ' ') desc++;
                int found = -1;
                for (int i = 0; i < gcount; i++) {
                    if (strcmp(groups[i].name, gname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    if (gcount >= gcap) {
                        gcap = gcap == 0 ? 16 : gcap * 2;
                        groups = realloc(groups, gcap * sizeof(Group));
                    }
                    groups[gcount].name = strdup(gname);
                    groups[gcount].events = NULL;
                    groups[gcount].count = 0;
                    groups[gcount].cap = 0;
                    found = gcount;
                    gcount++;
                }
                Group *g = &groups[found];
                if (g->count >= g->cap) {
                    g->cap = g->cap == 0 ? 16 : g->cap * 2;
                    g->events = realloc(g->events, g->cap * sizeof(char *));
                }
                g->events[g->count] = strdup(desc);
                g->count++;
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *gA = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *gB = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *gNew = p + 1;
                    while (*gNew == ' ') gNew++;
                    int idxA = -1, idxB = -1;
                    for (int i = 0; i < gcount; i++) {
                        if (strcmp(groups[i].name, gA) == 0) idxA = i;
                        if (strcmp(groups[i].name, gB) == 0) idxB = i;
                    }
                    int idxNew = -1;
                    for (int i = 0; i < gcount; i++) {
                        if (strcmp(groups[i].name, gNew) == 0) {
                            idxNew = i;
                            break;
                        }
                    }
                    if (idxNew != -1) {
                        for (int i = 0; i < groups[idxNew].count; i++) free(groups[idxNew].events[i]);
                        free(groups[idxNew].events);
                        groups[idxNew].events = NULL;
                        groups[idxNew].count = 0;
                        groups[idxNew].cap = 0;
                    } else {
                        if (gcount >= gcap) {
                            gcap = gcap == 0 ? 16 : gcap * 2;
                            groups = realloc(groups, gcap * sizeof(Group));
                        }
                        groups[gcount].name = strdup(gNew);
                        groups[gcount].events = NULL;
                        groups[gcount].count = 0;
                        groups[gcount].cap = 0;
                        idxNew = gcount;
                        gcount++;
                    }
                    Group *gN = &groups[idxNew];
                    if (idxA != -1) {
                        for (int i = 0; i < groups[idxA].count; i++) {
                            if (gN->count >= gN->cap) {
                                gN->cap = gN->cap == 0 ? 16 : gN->cap * 2;
                                gN->events = realloc(gN->events, gN->cap * sizeof(char *));
                            }
                            gN->events[gN->count] = strdup(groups[idxA].events[i]);
                            gN->count++;
                        }
                    }
                    if (idxB != -1) {
                        for (int i = 0; i < groups[idxB].count; i++) {
                            if (gN->count >= gN->cap) {
                                gN->cap = gN->cap == 0 ? 16 : gN->cap * 2;
                                gN->events = realloc(gN->events, gN->cap * sizeof(char *));
                            }
                            gN->events[gN->count] = strdup(groups[idxB].events[i]);
                            gN->count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETEGROUP ", 12) == 0) {
            char *gname = line + 12;
            while (*gname == ' ') gname++;
            int found = -1;
            for (int i = 0; i < gcount; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(groups[found].name);
                for (int i = 0; i < groups[found].count; i++) free(groups[found].events[i]);
                free(groups[found].events);
                for (int i = found; i < gcount - 1; i++) {
                    groups[i] = groups[i + 1];
                }
                gcount--;
            }
        } else if (strncmp(line, "COUNT ", 6) == 0) {
            char *gname = line + 6;
            while (*gname == ' ') gname++;
            int found = -1;
            for (int i = 0; i < gcount; i++) {
                if (strcmp(groups[i].name, gname) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%d\n", groups[found].count);
            else printf("0\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < gcount; i++) {
                printf("%s %d\n", groups[i].name, groups[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < gcount; i++) {
        free(groups[i].name);
        for (int j = 0; j < groups[i].count; j++) free(groups[i].events[j]);
        free(groups[i].events);
    }
    free(groups);
    return 0;
}
