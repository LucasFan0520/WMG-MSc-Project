/* F009.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **descriptions;
    int dcount;
    int dcap;
} Group;

typedef struct {
    char *name;
    Group group;
} GroupEntry;

static GroupEntry *groups = NULL;
static int gcount = 0;
static int gcap = 0;

static int ensure_gcap(void) {
    if (gcount >= gcap) {
        int newcap = gcap == 0 ? 8 : gcap * 2;
        GroupEntry *tmp = realloc(groups, sizeof(GroupEntry) * newcap);
        if (!tmp) return -1;
        groups = tmp;
        gcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_group(const char *name) {
    for (int i = 0; i < gcount; i++) {
        if (strcmp(groups[i].name, name) == 0)
            return i;
    }
    return -1;
}

static int group_add_desc(Group *g, const char *desc) {
    if (g->dcount >= g->dcap) {
        int newcap = g->dcap == 0 ? 4 : g->dcap * 2;
        char **tmp = realloc(g->descriptions, sizeof(char *) * newcap);
        if (!tmp) return -1;
        g->descriptions = tmp;
        g->dcap = newcap;
    }
    g->descriptions[g->dcount] = dup_str(desc);
    if (!g->descriptions[g->dcount]) return -1;
    g->dcount++;
    return 0;
}

static void free_group(Group *g) {
    for (int i = 0; i < g->dcount; i++)
        free(g->descriptions[i]);
    free(g->descriptions);
    g->descriptions = NULL;
    g->dcount = 0;
    g->dcap = 0;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "EVENT") == 0) {
            char grp[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", grp, &off2) != 1) continue;
            const char *desc = line + offset + off2;
            while (*desc == ' ') desc++;
            int idx = find_group(grp);
            if (idx < 0) {
                if (ensure_gcap() != 0) continue;
                groups[gcount].name = dup_str(grp);
                if (!groups[gcount].name) continue;
                groups[gcount].group.descriptions = NULL;
                groups[gcount].group.dcount = 0;
                groups[gcount].group.dcap = 0;
                idx = gcount;
                gcount++;
            }
            group_add_desc(&groups[idx].group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char ga[256] = {0}, gb[256] = {0}, gn[256] = {0};
            if (sscanf(line + offset, " %255s %255s %255s", ga, gb, gn) != 3) continue;
            int ia = find_group(ga);
            int ib = find_group(gb);
            if (ia < 0 || ib < 0) continue;
            if (find_group(gn) >= 0) continue;
            if (ensure_gcap() != 0) continue;
            groups[gcount].name = dup_str(gn);
            if (!groups[gcount].name) continue;
            groups[gcount].group.descriptions = NULL;
            groups[gcount].group.dcount = 0;
            groups[gcount].group.dcap = 0;
            for (int i = 0; i < groups[ia].group.dcount; i++)
                group_add_desc(&groups[gcount].group, groups[ia].group.descriptions[i]);
            for (int i = 0; i < groups[ib].group.dcount; i++)
                group_add_desc(&groups[gcount].group, groups[ib].group.descriptions[i]);
            gcount++;
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char grp[256] = {0};
            if (sscanf(line + offset, " %255s", grp) != 1) continue;
            int idx = find_group(grp);
            if (idx < 0) continue;
            free(groups[idx].name);
            free_group(&groups[idx].group);
            for (int i = idx; i < gcount - 1; i++)
                groups[i] = groups[i + 1];
            gcount--;
        } else if (strcmp(cmd, "COUNT") == 0) {
            char grp[256] = {0};
            if (sscanf(line + offset, " %255s", grp) != 1) continue;
            int idx = find_group(grp);
            if (idx >= 0) {
                printf("%d\n", groups[idx].group.dcount);
            } else {
                printf("0\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < gcount; i++) {
                printf("%s:", groups[i].name);
                for (int j = 0; j < groups[i].group.dcount; j++) {
                    printf(" %s", groups[i].group.descriptions[j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < gcount; i++) {
        free(groups[i].name);
        free_group(&groups[i].group);
    }
    free(groups);
    return 0;
}