// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t lcount;
    size_t lcap;
} Item;

typedef struct {
    Item *items;
    size_t count;
    size_t capacity;
} Tracker;

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

static void item_free(Item *it) {
    free(it->name);
    free(it->note);
    for (size_t i = 0; i < it->lcount; i++)
        free(it->labels[i]);
    free(it->labels);
    it->name = NULL;
    it->note = NULL;
    it->labels = NULL;
    it->lcount = 0;
    it->lcap = 0;
}

static void tr_init(Tracker *tr) {
    tr->items = NULL;
    tr->count = 0;
    tr->capacity = 0;
}

static void tr_free(Tracker *tr) {
    for (size_t i = 0; i < tr->count; i++)
        item_free(&tr->items[i]);
    free(tr->items);
    tr->items = NULL;
    tr->count = 0;
    tr->capacity = 0;
}

static int tr_find(Tracker *tr, const char *name) {
    for (size_t i = 0; i < tr->count; i++) {
        if (strcmp(tr->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void tr_item(Tracker *tr, const char *name, const char *note) {
    if (tr_find(tr, name) >= 0) return;
    if (tr->count == tr->capacity) {
        size_t newcap = tr->capacity ? tr->capacity * 2 : 4;
        Item *tmp = realloc(tr->items, newcap * sizeof(Item));
        if (!tmp) return;
        tr->items = tmp;
        tr->capacity = newcap;
    }
    Item *it = &tr->items[tr->count];
    it->name = safe_strdup(name);
    it->note = safe_strdup(note);
    it->labels = NULL;
    it->lcount = 0;
    it->lcap = 0;
    if (it->name && it->note)
        tr->count++;
    else {
        free(it->name);
        free(it->note);
    }
}

static void tr_label(Tracker *tr, const char *name, const char *label) {
    int idx = tr_find(tr, name);
    if (idx < 0) return;
    Item *it = &tr->items[idx];
    for (size_t i = 0; i < it->lcount; i++) {
        if (strcmp(it->labels[i], label) == 0)
            return;
    }
    if (it->lcount == it->lcap) {
        size_t newcap = it->lcap ? it->lcap * 2 : 4;
        char **tmp = realloc(it->labels, newcap * sizeof(char *));
        if (!tmp) return;
        it->labels = tmp;
        it->lcap = newcap;
    }
    it->labels[it->lcount] = safe_strdup(label);
    if (it->labels[it->lcount])
        it->lcount++;
}

static void tr_unlabel(Tracker *tr, const char *name, const char *label) {
    int idx = tr_find(tr, name);
    if (idx < 0) return;
    Item *it = &tr->items[idx];
    size_t write = 0;
    for (size_t i = 0; i < it->lcount; i++) {
        if (strcmp(it->labels[i], label) != 0) {
            if (write != i) it->labels[write] = it->labels[i];
            write++;
        } else {
            free(it->labels[i]);
        }
    }
    it->lcount = write;
}

static void tr_delete(Tracker *tr, const char *name) {
    int idx = tr_find(tr, name);
    if (idx < 0) return;
    item_free(&tr->items[idx]);
    for (size_t i = idx; i < tr->count - 1; i++)
        tr->items[i] = tr->items[i+1];
    tr->count--;
}

static void tr_find_print(Tracker *tr, const char *name) {
    int idx = tr_find(tr, name);
    if (idx < 0) return;
    Item *it = &tr->items[idx];
    const char *p = it->note;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    printf(" labels:");
    for (size_t i = 0; i < it->lcount; i++)
        printf(" %s", it->labels[i]);
    putchar('\n');
}

static void tr_report(Tracker *tr) {
    for (size_t i = 0; i < tr->count; i++) {
        printf("%s\n", tr->items[i].name);
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
    Tracker tr;
    tr_init(&tr);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "ITEM") == 0) {
            tr_item(&tr, a1, a2);
        } else if (strcmp(cmd, "LABEL") == 0) {
            tr_label(&tr, a1, a2);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            tr_unlabel(&tr, a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            tr_delete(&tr, a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            tr_find_print(&tr, a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            tr_report(&tr);
        }
    }
    tr_free(&tr);
    return 0;
}