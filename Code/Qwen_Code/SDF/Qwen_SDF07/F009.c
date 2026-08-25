// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *group; char **descs; size_t dc; size_t dcap; } Grp;
Grp *grps = NULL;
size_t gc = 0, gcap = 0;

Grp* find_grp(const char *group) {
    for (size_t i = 0; i < gc; i++) if (strcmp(grps[i].group, group) == 0) return &grps[i];
    return NULL;
}

Grp* add_grp(const char *group) {
    if (gc == gcap) {
        size_t nc = gcap ? gcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Grp)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Grp *tmp = realloc(grps, nc * sizeof(Grp));
        if (!tmp) { perror("realloc"); exit(1); }
        grps = tmp;
        gcap = nc;
    }
    grps[gc].group = strdup(group);
    grps[gc].descs = NULL;
    grps[gc].dc = 0;
    grps[gc].dcap = 0;
    if (!grps[gc].group) { perror("strdup"); exit(1); }
    return &grps[gc++];
}

void add_desc(Grp *g, const char *desc) {
    if (g->dc == g->dcap) {
        size_t nc = g->dcap ? g->dcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(g->descs, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        g->descs = tmp;
        g->dcap = nc;
    }
    g->descs[g->dc] = strdup(desc);
    if (!g->descs[g->dc]) { perror("strdup"); exit(1); }
    g->dc++;
}

void del_grp(const char *group) {
    for (size_t i = 0; i < gc; i++) {
        if (strcmp(grps[i].group, group) == 0) {
            free(grps[i].group);
            for (size_t j = 0; j < grps[i].dc; j++) free(grps[i].descs[j]);
            free(grps[i].descs);
            grps[i] = grps[--gc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256];
            if (sscanf(line, "%*s %255s", group) != 1) { free(line); continue; }
            char *desc = strchr(line, ' ');
            if (desc) { desc = strchr(desc + 1, ' '); if (desc) desc++; }
            if (!desc) desc = "";
            Grp *g = find_grp(group);
            if (!g) g = add_grp(group);
            add_desc(g, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char a[256], b[256], n[256];
            if (sscanf(line, "%*s %255s %255s %255s", a, b, n) == 3) {
                Grp *ga = find_grp(a), *gb = find_grp(b);
                if (ga && gb) {
                    Grp *gn = find_grp(n);
                    if (!gn) gn = add_grp(n);
                    for (size_t i = 0; i < ga->dc; i++) add_desc(gn, ga->descs[i]);
                    for (size_t i = 0; i < gb->dc; i++) add_desc(gn, gb->descs[i]);
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char group[256];
            if (sscanf(line, "%*s %255s", group) == 1) del_grp(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char group[256];
            if (sscanf(line, "%*s %255s", group) == 1) {
                Grp *g = find_grp(group);
                printf("%zu\n", g ? g->dc : 0);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < gc; i++) {
                printf("%s:", grps[i].group);
                for (size_t j = 0; j < grps[i].dc; j++) printf(" %s", grps[i].descs[j]);
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < gc; i++) {
        free(grps[i].group);
        for (size_t j = 0; j < grps[i].dc; j++) free(grps[i].descs[j]);
        free(grps[i].descs);
    }
    free(grps);
    return 0;
}