// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label { char *label; struct Label *next; } Label;
typedef struct Item { char *name; char *note; Label *labels; struct Item *next; } Item;
Item *items = NULL;

Item *find_item(const char *name) {
    for (Item *i = items; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name); i->note = strdup(note); i->labels = NULL; i->next = items; items = i;
}

void label_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    for (Label *l = i->labels; l; l = l->next)
        if (strcmp(l->label, label) == 0) return;
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL;
    for (Label *l = i->labels; l; prev = l, l = l->next) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next; else i->labels = l->next;
            free(l->label); free(l);
            return;
        }
    }
}

void delete_item(const char *name) {
    Item *prev = NULL;
    for (Item *i = items; i; prev = i, i = i->next) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next; else items = i->next;
            free(i->name); free(i->note);
            while (i->labels) { Label *l = i->labels; i->labels = l->next; free(l->label); free(l); }
            free(i);
            return;
        }
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", i->note);
    for (Label *l = i->labels; l; l = l->next) printf("%s\n", l->label);
}

void report_items(void) {
    for (Item *i = items; i; i = i->next) {
        printf("%s\n%s\n", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) printf("%s\n", l->label);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *n = rest + strlen(name);
                while (*n == ' ') n++;
                strcpy(note, n);
                item_create(name, note);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) == 2) label_item(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) == 2) unlabel_item(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    while (items) { Item *i = items; items = i->next; free(i->name); free(i->note); while (i->labels) { Label *l = i->labels; i->labels = l->next; free(l->label); free(l); } free(i); }
    return 0;
}