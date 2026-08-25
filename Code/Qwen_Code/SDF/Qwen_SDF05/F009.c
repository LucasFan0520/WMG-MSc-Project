// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char **descs;
    size_t dcnt, dcap;
} Group;

typedef struct { char *name; Group g; } GroupEntry;
static GroupEntry *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    GroupEntry *t = realloc(arr, nc * sizeof(GroupEntry));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int group_grow(Group *g) {
    if (g->dcnt < g->dcap) return 0;
    size_t nc = g->dcap ? g->dcap * 2 : 4;
    if (nc < g->dcap) return -1;
    char **t = realloc(g->descs, nc * sizeof(char *));
    if (!t) return -1;
    g->descs = t; g->dcap = nc;
    return 0;
}

static int find_group(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_group(Group *g) {
    for (size_t i = 0; i < g->dcnt; i++) free(g->descs[i]);
    free(g->descs);
    g->descs = NULL; g->dcnt = 0; g->dcap = 0;
}

static void cmd_event(char *group, char *desc) {
    int idx = find_group(group);
    if (idx < 0) {
        if (grow() < 0) return;
        arr[cnt].name = safe_strdup(group);
        if (!arr[cnt].name) return;
        arr[cnt].g.descs = NULL;
        arr[cnt].g.dcnt = 0;
        arr[cnt].g.dcap = 0;
        idx = (int)cnt;
        cnt++;
    }
    Group *g = &arr[idx].g;
    if (group_grow(g) < 0) return;
    g->descs[g->dcnt] = safe_strdup(desc);
    if (!g->descs[g->dcnt]) return;
    g->dcnt++;
}

static void cmd_merge(char *gA, char *gB, char *newG) {
    int iA = find_group(gA);
    int iB = find_group(gB);
    if (iA < 0 || iB < 0) return;
    int iN = find_group(newG);
    if (iN < 0) {
        if (grow() < 0) return;
        arr[cnt].name = safe_strdup(newG);
        if (!arr[cnt].name) return;
        arr[cnt].g.descs = NULL;
        arr[cnt].g.dcnt = 0;
        arr[cnt].g.dcap = 0;
        iN = (int)cnt;
        cnt++;
    }
    Group *gN = &arr[iN].g;
    Group *aA = &arr[iA].g;
    Group *aB = &arr[iB].g;
    for (size_t i = 0; i < aA->dcnt; i++) {
        if (group_grow(gN) < 0) return;
        gN->descs[gN->dcnt] = safe_strdup(aA->descs[i]);
        if (gN->descs[gN->dcnt]) gN->dcnt++;
    }
    for (size_t i = 0; i < aB->dcnt; i++) {
        if (group_grow(gN) < 0) return;
        gN->descs[gN->dcnt] = safe_strdup(aB->descs[i]);
        if (gN->descs[gN->dcnt]) gN->dcnt++;
    }
}

static void cmd_deletegroup(char *group) {
    int idx = find_group(group);
    if (idx < 0) return;
    free(arr[idx].name);
    free_group(&arr[idx].g);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_count(char *group) {
    int idx = find_group(group);
    if (idx < 0) { printf("0\n"); return; }
    printf("%zu\n", arr[idx].g.dcnt);
}

static void cmd_report(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s:", arr[i].name);
        for (size_t j = 0; j < arr[i].g.dcnt; j++) {
            if (j > 0) printf(" ");
            printf("%s", arr[i].g.descs[j]);
        }
        putchar('\n');
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) {
        free(arr[i].name);
        free_group(&arr[i].g);
    }
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "EVENT") == 0) {
            char group[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", group, &n2) < 1) continue;
            char *desc = rest + n2;
            while (*desc == ' ') desc++;
            cmd_event(group, desc);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char gA[256] = {0}, gB[256] = {0}, nG[256] = {0};
            if (sscanf(rest, "%255s %255s %255s", gA, gB, nG) < 3) continue;
            cmd_merge(gA, gB, nG);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char group[256] = {0};
            if (sscanf(rest, "%255s", group) < 1) continue;
            cmd_deletegroup(group);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char group[256] = {0};
            if (sscanf(rest, "%255s", group) < 1) continue;
            cmd_count(group);
        } else if (strcmp(cmd, "REPORT") == 0) {
            cmd_report();
        }
    }
    cleanup();
    return 0;
}