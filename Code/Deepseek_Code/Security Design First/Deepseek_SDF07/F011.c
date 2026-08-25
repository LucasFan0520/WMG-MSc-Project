// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *title;
    char *body;
} Draft;

typedef struct {
    Draft *items;
    size_t count;
    size_t capacity;
} DraftManager;

typedef struct {
    char *title;
    char *body;
    int available;
} Undo;

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

static void dm_init(DraftManager *dm) {
    dm->items = NULL;
    dm->count = 0;
    dm->capacity = 0;
}

static void dm_free(DraftManager *dm) {
    for (size_t i = 0; i < dm->count; i++) {
        free(dm->items[i].title);
        free(dm->items[i].body);
    }
    free(dm->items);
    dm->items = NULL;
    dm->count = 0;
    dm->capacity = 0;
}

static int dm_find(DraftManager *dm, const char *title) {
    for (size_t i = 0; i < dm->count; i++) {
        if (strcmp(dm->items[i].title, title) == 0)
            return (int)i;
    }
    return -1;
}

static void dm_create(DraftManager *dm, const char *title, const char *body) {
    if (dm_find(dm, title) >= 0) return;
    if (dm->count == dm->capacity) {
        size_t newcap = dm->capacity ? dm->capacity * 2 : 4;
        Draft *tmp = realloc(dm->items, newcap * sizeof(Draft));
        if (!tmp) return;
        dm->items = tmp;
        dm->capacity = newcap;
    }
    dm->items[dm->count].title = safe_strdup(title);
    dm->items[dm->count].body = safe_strdup(body);
    if (dm->items[dm->count].title && dm->items[dm->count].body)
        dm->count++;
    else {
        free(dm->items[dm->count].title);
        free(dm->items[dm->count].body);
    }
}

static void dm_revise(DraftManager *dm, const char *title, const char *body) {
    int idx = dm_find(dm, title);
    if (idx < 0) return;
    char *new = safe_strdup(body);
    if (!new) return;
    free(dm->items[idx].body);
    dm->items[idx].body = new;
}

static void dm_discard(DraftManager *dm, Undo *undo, const char *title) {
    int idx = dm_find(dm, title);
    if (idx < 0) return;
    free(undo->title);
    free(undo->body);
    undo->title = safe_strdup(dm->items[idx].title);
    undo->body = safe_strdup(dm->items[idx].body);
    undo->available = (undo->title && undo->body) ? 1 : 0;
    free(dm->items[idx].title);
    free(dm->items[idx].body);
    for (size_t i = idx; i < dm->count - 1; i++)
        dm->items[i] = dm->items[i+1];
    dm->count--;
}

static void dm_restore(DraftManager *dm, Undo *undo) {
    if (!undo->available) return;
    dm_create(dm, undo->title, undo->body);
    free(undo->title);
    free(undo->body);
    undo->title = NULL;
    undo->body = NULL;
    undo->available = 0;
}

static void dm_find_print(DraftManager *dm, const char *title) {
    int idx = dm_find(dm, title);
    if (idx < 0) return;
    printf("%s\n", dm->items[idx].body);
}

static void dm_print_all(DraftManager *dm) {
    for (size_t i = 0; i < dm->count; i++) {
        printf("%s\n", dm->items[i].title);
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
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
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    DraftManager dm;
    dm_init(&dm);
    Undo undo = {NULL, NULL, 0};
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "CREATE") == 0) {
            dm_create(&dm, a1, a2);
        } else if (strcmp(cmd, "REVISE") == 0) {
            dm_revise(&dm, a1, a2);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            dm_discard(&dm, &undo, a1);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            dm_restore(&dm, &undo);
        } else if (strcmp(cmd, "FIND") == 0) {
            dm_find_print(&dm, a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            dm_print_all(&dm);
        }
    }
    dm_free(&dm);
    free(undo.title);
    free(undo.body);
    return 0;
}