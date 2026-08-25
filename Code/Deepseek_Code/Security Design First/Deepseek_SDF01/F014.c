// filename: F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LabelNode {
    char *label;
    struct LabelNode *next;
} LabelNode;

typedef struct Item {
    char *name;
    char *note;
    LabelNode *labels;
    struct Item *next;
} Item;

static Item *items = NULL;

static void free_labels(LabelNode *l) {
    while (l) {
        LabelNode *tmp = l;
        l = l->next;
        free(tmp->label);
        free(tmp);
    }
}

static void cleanup(void) {
    while (items) {
        Item *tmp = items;
        items = items->next;
        free(tmp->name);
        free(tmp->note);
        free_labels(tmp->labels);
        free(tmp);
    }
}

static Item *find_item(const char *name) {
    for (Item *it = items; it; it = it->next)
        if (strcmp(it->name, name) == 0)
            return it;
    return NULL;
}

static void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *it = malloc(sizeof(Item));
    if (!it) return;
    it->name = strdup(name);
    it->note = strdup(note);
    if (!it->name || !it->note) {
        free(it->name);
        free(it->note);
        free(it);
        return;
    }
    it->labels = NULL;
    it->next = items;
    items = it;
}

static void add_label(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    // avoid duplicate label?
    for (LabelNode *l = it->labels; l; l = l->next)
        if (strcmp(l->label, label) == 0)
            return;
    LabelNode *ln = malloc(sizeof(LabelNode));
    if (!ln) return;
    ln->label = strdup(label);
    if (!ln->label) { free(ln); return; }
    ln->next = it->labels;
    it->labels = ln;
}

static void unlabel(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    LabelNode *prev = NULL;
    for (LabelNode *l = it->labels; l; l = l->next) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next;
            else it->labels = l->next;
            free(l->label);
            free(l);
            return;
        }
        prev = l;
    }
}

static void delete_item(const char *name) {
    Item *prev = NULL;
    for (Item *it = items; it; it = it->next) {
        if (strcmp(it->name, name) == 0) {
            if (prev) prev->next = it->next;
            else items = it->next;
            free(it->name);
            free(it->note);
            free_labels(it->labels);
            free(it);
            return;
        }
        prev = it;
    }
}

static void find_print(const char *name) {
    Item *it = find_item(name);
    if (!it) return;
    printf("%s %s", it->name, it->note);
    for (LabelNode *l = it->labels; l; l = l->next)
        printf(" %s", l->label);
    putchar('\n');
}

static void report_items(void) {
    for (Item *it = items; it; it = it->next) {
        printf("%s %s", it->name, it->note);
        for (LabelNode *l = it->labels; l; l = l->next)
            printf(" %s", l->label);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "ITEM") == 0) {
            if (*arg1) create_item(arg1, arg2);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (*arg1 && *arg2) add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (*arg1 && *arg2) unlabel(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free(line);
    return 0;
}