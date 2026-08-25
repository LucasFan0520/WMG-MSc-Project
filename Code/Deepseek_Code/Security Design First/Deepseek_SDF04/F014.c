// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    char *label;
    struct Label *next;
} Label;

typedef struct Item {
    char *name;
    char *note;
    Label *labels;
    struct Item *next;
} Item;

Item *head = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = NULL;
    if (!i->name || !i->note) {
        free(i->name); free(i->note); free(i);
        return;
    }
    if (!head) head = i;
    else {
        Item *last = head;
        while (last->next) last = last->next;
        last->next = i;
    }
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels;
    while (l) {
        if (strcmp(l->label, label) == 0) return;
        l = l->next;
    }
    Label *nl = malloc(sizeof(Label));
    if (!nl) return;
    nl->label = strdup(label);
    nl->next = NULL;
    if (!nl->label) { free(nl); return; }
    if (!i->labels) i->labels = nl;
    else {
        Label *last = i->labels;
        while (last->next) last = last->next;
        last->next = nl;
    }
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
    while (l) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->label);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            free(i->name);
            free(i->note);
            Label *l = i->labels;
            while (l) {
                Label *next = l->next;
                free(l->label);
                free(l);
                l = next;
            }
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
    printf("%s %s\n", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf("%s\n", l->label);
        l = l->next;
    }
}

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        Label *l = i->labels;
        while (l) {
            printf("%s\n", l->label);
            l = l->next;
        }
        i = i->next;
    }
}

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i->note);
        Label *l = i->labels;
        while (l) {
            Label *ln = l->next;
            free(l->label);
            free(l);
            l = ln;
        }
        free(i);
        i = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], note[4096], label[256];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                create_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, label) == 2)
                add_label(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, label) == 2)
                unlabel_item(name, label);
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
    free(line);
    free_all();
    return 0;
}