// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    char *name;
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
    Item *curr = items;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
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

void add_label(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *curr = i->labels;
    while (curr) {
        if (strcmp(curr->name, label_name) == 0) return;
        curr = curr->next;
    }
    Label *l = malloc(sizeof(Label));
    l->name = strdup(label_name);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *curr = i->labels, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, label_name) == 0) {
            if (prev) prev->next = curr->next;
            else i->labels = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_item(const char *name) {
    Item *curr = items, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else items = curr->next;
            Label *l = curr->labels;
            while (l) {
                Label *next = l->next;
                free(l->name);
                free(l);
                l = next;
            }
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) return;
    printf("%s %s", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf(" %s", l->name);
        l = l->next;
    }
    printf("\n");
}

void report_items() {
    Item *curr = items;
    while (curr) {
        printf("%s %s", curr->name, curr->note);
        Label *l = curr->labels;
        while (l) {
            printf(" %s", l->name);
            l = l->next;
        }
        printf("\n");
        curr = curr->next;
    }
}

void free_all() {
    Item *curr = items;
    while (curr) {
        Item *next = curr->next;
        Label *l = curr->labels;
        while (l) {
            Label *next_l = l->next;
            free(l->name);
            free(l);
            l = next_l;
        }
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], name[256], arg2[256], note[4096] = {0};
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, name, arg2, note);
        
        if (strcmp(cmd, "ITEM") == 0 && n >= 2) create_item(name, arg2);
        else if (strcmp(cmd, "LABEL") == 0 && n >= 2) add_label(name, arg2);
        else if (strcmp(cmd, "UNLABEL") == 0 && n >= 2) unlabel_item(name, arg2);
        else if (strcmp(cmd, "DELETE") == 0 && n >= 1) delete_item(name);
        else if (strcmp(cmd, "FIND") == 0 && n >= 1) find_item_print(name);
        else if (strcmp(cmd, "REPORT") == 0) report_items();
    }
    free_all();
    return 0;
}