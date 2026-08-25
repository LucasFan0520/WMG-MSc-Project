// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct label {
    char *text;
    struct label *next;
} Label;

typedef struct item {
    char *name;
    char *note;
    Label *labels;
    struct item *next;
} Item;

Item *items = NULL;

Item *find_item(const char *name) {
    Item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(Item *i, const char *label) {
    Label *l = malloc(sizeof(Label));
    l->text = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void item_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    add_label(i, label);
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
    while (l) {
        if (strcmp(l->text, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->text);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            Label *l = i->labels;
            while (l) { Label *tmp = l; l = l->next; free(tmp->text); free(tmp); }
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) return;
    printf("%s %s", i->name, i->note);
    Label *l = i->labels;
    while (l) { printf(" %s", l->text); l = l->next; }
    putchar('\n');
}

void report_items(void) {
    Item *i = items;
    while (i) {
        printf("%s %s", i->name, i->note);
        Label *l = i->labels;
        while (l) { printf(" %s", l->text); l = l->next; }
        putchar('\n');
        i = i->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], arg[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, arg) == 2)
                item_create(name, arg);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, arg) == 2)
                item_label(name, arg);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, arg) == 2)
                unlabel_item(name, arg);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}