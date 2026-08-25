// F011.c
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

typedef struct { char *title; char *body; } Draft;
static Draft *drafts = NULL;
static size_t dcnt = 0, dcap = 0;

typedef struct { char *title; char *body; } Discarded;
static Discarded *disc = NULL;
static size_t disc_cnt = 0, disc_cap = 0;

static int dgrow(void) {
    if (dcnt < dcap) return 0;
    size_t nc = dcap ? dcap * 2 : 8;
    if (nc < dcap) return -1;
    Draft *t = realloc(drafts, nc * sizeof(Draft));
    if (!t) return -1;
    drafts = t; dcap = nc;
    return 0;
}

static int disc_grow(void) {
    if (disc_cnt < disc_cap) return 0;
    size_t nc = disc_cap ? disc_cap * 2 : 4;
    if (nc < disc_cap) return -1;
    Discarded *t = realloc(disc, nc * sizeof(Discarded));
    if (!t) return -1;
    disc = t; disc_cap = nc;
    return 0;
}

static int find_draft(const char *title) {
    for (size_t i = 0; i < dcnt; i++)
        if (strcmp(drafts[i].title, title) == 0) return (int)i;
    return -1;
}

static void cmd_create(char *title, char *body) {
    if (find_draft(title) >= 0) return;
    if (dgrow() < 0) return;
    drafts[dcnt].title = safe_strdup(title);
    drafts[dcnt].body = safe_strdup(body);
    if (!drafts[dcnt].title || !drafts[dcnt].body) {
        free(drafts[dcnt].title); free(drafts[dcnt].body);
        drafts[dcnt].title = NULL; drafts[dcnt].body = NULL;
        return;
    }
    dcnt++;
}

static void cmd_revise(char *title, char *body) {
    int idx = find_draft(title);
    if (idx < 0) return;
    char *nb = safe_strdup(body);
    if (!nb) return;
    free(drafts[idx].body);
    drafts[idx].body = nb;
}

static void cmd_discard(char *title) {
    int idx = find_draft(title);
    if (idx < 0) return;
    if (disc_grow() < 0) return;
    disc[disc_cnt].title = drafts[idx].title;
    disc[disc_cnt].body = drafts[idx].body;
    disc_cnt++;
    for (size_t i = (size_t)idx; i + 1 < dcnt; i++) drafts[i] = drafts[i + 1];
    dcnt--;
}

static void cmd_restore(void) {
    if (disc_cnt == 0) return;
    disc_cnt--;
    if (find_draft(disc[disc_cnt].title) >= 0) {
        free(disc[disc_cnt].title); free(disc[disc_cnt].body);
        return;
    }
    if (dgrow() < 0) {
        free(disc[disc_cnt].title); free(disc[disc_cnt].body);
        return;
    }
    drafts[dcnt].title = disc[disc_cnt].title;
    drafts[dcnt].body = disc[disc_cnt].body;
    dcnt++;
}

static void cmd_find(char *title) {
    int idx = find_draft(title);
    if (idx < 0) return;
    printf("%s %s\n", drafts[idx].title, drafts[idx].body);
}

static void cmd_print(void) {
    for (size_t i = 0; i < dcnt; i++)
        printf("%s %s\n", drafts[i].title, drafts[i].body);
}

static void cleanup(void) {
    for (size_t i = 0; i < dcnt; i++) { free(drafts[i].title); free(drafts[i].body); }
    free(drafts);
    for (size_t i = 0; i < disc_cnt; i++) { free(disc[i].title); free(disc[i].body); }
    free(disc);
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
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char title[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", title, &n2) < 1) continue;
            char *body = rest + n2;
            while (*body == ' ') body++;
            if (strcmp(cmd, "CREATE") == 0) cmd_create(title, body);
            else cmd_revise(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256] = {0};
            if (sscanf(rest, "%255s", title) < 1) continue;
            cmd_discard(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            cmd_restore();
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(rest, "%255s", title) < 1) continue;
            cmd_find(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}