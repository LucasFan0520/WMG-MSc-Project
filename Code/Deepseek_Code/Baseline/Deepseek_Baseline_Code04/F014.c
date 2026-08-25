/* F014.c */
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

void add_label(Item *i, const char *label) {
    Label *l = malloc(sizeof(Label));
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel(Item *i, const char *label) {
    Label *prev = NULL, *cur = i->labels;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else i->labels = cur->next;
            free(cur->label);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL, *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else items = cur->next;
            free(cur->name);
            free(cur->note);
            Label *l = cur->labels;
            while (l) {
                Label *tmp = l;
                l = l->next;
                free(tmp->label);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_item(Item *i) {
    printf("%s %s\n", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf("%s\n", l->label);
        l = l->next;
    }
}

void print_all(void) {
    Item *i = items;
    while (i) {
        printf("%s\n", i->name);
        i = i->next;
    }
}

void free_items(void) {
    while (items) {
        Item *tmp = items;
        items = items->next;
        free(tmp->name);
        free(tmp->note);
        Label *l = tmp->labels;
        while (l) {
            Label *tl = l;
            l = l->next;
            free(tl->label);
            free(tl);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], label[256];
        int n = sscanf(line, "%s %s %s", cmd, name, label);
        if (strcmp(cmd, "ITEM") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) create_item(name, p);
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (n < 3) continue;
            Item *i = find_item(name);
            if (i) add_label(i, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (n < 3) continue;
            Item *i = find_item(name);
            if (i) unlabel(i, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            Item *i = find_item(name);
            if (i) print_item(i);
        } else if (strcmp(cmd, "REPORT") == 0) {
            print_all();
        }
    }
    free_items();
    return 0;
}