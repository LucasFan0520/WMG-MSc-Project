/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct label {
    char *label;
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

void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels;
    while (l) { if (strcmp(l->label, label) == 0) return; l = l->next; }
    Label *nl = malloc(sizeof(Label));
    nl->label = strdup(label);
    nl->next = i->labels;
    i->labels = nl;
}

void remove_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
    while (l) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->label); free(l);
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
            free(i->name); free(i->note);
            Label *l = i->labels;
            while (l) { Label *tmp = l; l = l->next; free(tmp->label); free(tmp); }
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) { printf("NOT_FOUND\n"); return; }
    printf("%s ", i->name);
    for (char *c = i->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar(' ');
    Label *l = i->labels;
    while (l) {
        for (char *c = l->label; *c; c++) putchar(*c == ' ' ? '_' : *c);
        if (l->next) putchar(' ');
        l = l->next;
    }
    putchar('\n');
}

void report_items() {
    Item *i = items;
    while (i) {
        printf("%s ", i->name);
        for (char *c = i->note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar(' ');
        Label *l = i->labels;
        while (l) {
            for (char *c = l->label; *c; c++) putchar(*c == ' ' ? '_' : *c);
            if (l->next) putchar(' ');
            l = l->next;
        }
        putchar('\n');
        i = i->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], label[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_items(); continue; }
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            create_item(name, rest);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) add_label(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) remove_label(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_item_print(name);
        }
    }
    return 0;
}