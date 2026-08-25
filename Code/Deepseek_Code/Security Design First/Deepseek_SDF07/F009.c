// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char **descs;
    size_t count;
    size_t capacity;
} EventList;

typedef struct {
    char *name;
    EventList events;
} Group;

typedef struct {
    Group *items;
    size_t count;
    size_t capacity;
} GroupSet;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void el_init(EventList *el) {
    el->descs = NULL;
    el->count = 0;
    el->capacity = 0;
}

static void el_free(EventList *el) {
    for (size_t i = 0; i < el->count; i++)
        free(el->descs[i]);
    free(el->descs);
    el->descs = NULL;
    el->count = 0;
    el->capacity = 0;
}

static void el_add(EventList *el, const char *desc) {
    if (el->count == el->capacity) {
        size_t newcap = el->capacity ? el->capacity * 2 : 4;
        char **tmp = realloc(el->descs, newcap * sizeof(char *));
        if (!tmp) return;
        el->descs = tmp;
        el->capacity = newcap;
    }
    el->descs[el->count] = safe_strdup(desc);
    if (el->descs[el->count])
        el->count++;
}

static void gs_init(GroupSet *gs) {
    gs->items = NULL;
    gs->count = 0;
    gs->capacity = 0;
}

static void gs_free(GroupSet *gs) {
    for (size_t i = 0; i < gs->count; i++) {
        free(gs->items[i].name);
        el_free(&gs->items[i].events);
    }
    free(gs->items);
    gs->items = NULL;
    gs->count = 0;
    gs->capacity = 0;
}

static int gs_find(GroupSet *gs, const char *name) {
    for (size_t i = 0; i < gs->count; i++) {
        if (strcmp(gs->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void gs_event(GroupSet *gs, const char *group, const char *desc) {
    int idx = gs_find(gs, group);
    if (idx < 0) {
        if (gs->count == gs->capacity) {
            size_t newcap = gs->capacity ? gs->capacity * 2 : 4;
            Group *tmp = realloc(gs->items, newcap * sizeof(Group));
            if (!tmp) return;
            gs->items = tmp;
            gs->capacity = newcap;
        }
        gs->items[gs->count].name = safe_strdup(group);
        el_init(&gs->items[gs->count].events);
        if (!gs->items[gs->count].name) {
            gs->items[gs->count].name = NULL;
            return;
        }
        idx = (int)gs->count;
        gs->count++;
    }
    el_add(&gs->items[idx].events, desc);
}

static void gs_merge(GroupSet *gs, const char *a, const char *b, const char *newg) {
    int ia = gs_find(gs, a);
    int ib = gs_find(gs, b);
    if (ia < 0 || ib < 0) return;
    if (gs_find(gs, newg) >= 0) return;
    if (gs->count == gs->capacity) {
        size_t newcap = gs->capacity ? gs->capacity * 2 : 4;
        Group *tmp = realloc(gs->items, newcap * sizeof(Group));
        if (!tmp) return;
        gs->items = tmp;
        gs->capacity = newcap;
    }
    gs->items[gs->count].name = safe_strdup(newg);
    el_init(&gs->items[gs->count].events);
    if (!gs->items[gs->count].name) {
        gs->items[gs->count].name = NULL;
        return;
    }
    for (size_t i = 0; i < gs->items[ia].events.count; i++)
        el_add(&gs->items[gs->count].events, gs->items[ia].events.descs[i]);
    for (size_t i = 0; i < gs->items[ib].events.count; i++)
        el_add(&gs->items[gs->count].events, gs->items[ib].events.descs[i]);
    gs->count++;
}

static void gs_deletegroup(GroupSet *gs, const char *group) {
    int idx = gs_find(gs, group);
    if (idx < 0) return;
    free(gs->items[idx].name);
    el_free(&gs->items[idx].events);
    for (size_t i = idx; i < gs->count - 1; i++)
        gs->items[i] = gs->items[i+1];
    gs->count--;
}

static void gs_count(GroupSet *gs, const char *group) {
    int idx = gs_find(gs, group);
    printf("%zu\n", idx >= 0 ? gs->items[idx].events.count : 0);
}

static void gs_report(GroupSet *gs) {
    for (size_t i = 0; i < gs->count; i++) {
        printf("%s:", gs->items[i].name);
        for (size_t j = 0; j < gs->items[i].events.count; j++) {
            printf(" %s", gs->items[i].events.descs[j]);
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    GroupSet gs;
    gs_init(&gs);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "EVENT") == 0) {
            gs_event(&gs, a1, a2); // a1=group, a2=description
        } else if (strcmp(cmd, "MERGE") == 0) {
            gs_merge(&gs, a1, a2, a3);
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            gs_deletegroup(&gs, a1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            gs_count(&gs, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            gs_report(&gs);
        }
    }
    gs_free(&gs);
    return 0;
}