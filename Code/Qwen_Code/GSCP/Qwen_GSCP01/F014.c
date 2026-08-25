// F014.c
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
    if (!i) exit(EXIT_FAILURE);
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    LabelNode *curr = i->labels;
    while (curr) {
        if (strcmp(curr->label, label) == 0) return;
        curr = curr->next;
    }
    LabelNode *l = malloc(sizeof(LabelNode));
    if (!l) exit(EXIT_FAILURE);
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    LabelNode *curr = i->labels;
    LabelNode *prev = NULL;
    while (curr) {
        if (strcmp(curr->label, label) == 0) {
            if (prev) prev->next = curr->next;
            else i->labels = curr->next;
            free(curr->label);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_item(const char *name) {
    Item *curr = items;
    Item *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else items = curr->next;
            LabelNode *l = curr->labels;
            while (l) {
                LabelNode *next_l = l->next;
                free(l->label);
                free(l);
                l = next_l;
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
    LabelNode *l = i->labels;
    while (l) {
        printf(" %s", l->label);
        l = l->next;
    }
    printf("\n");
}

void report_items() {
    Item *curr = items;
    while (curr) {
        find_item_print(curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Item *curr = items;
    while (curr) {
        Item *next_i = curr->next;
        LabelNode *l = curr->labels;
        while (l) {
            LabelNode *next_l = l->next;
            free(l->label);
            free(l);
            l = next_l;
        }
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next_i;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        char arg3[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "ITEM") == 0 && matched >= 3) {
            char *note_start = strchr(line, ' ');
            if (note_start) {
                note_start = strchr(note_start + 1, ' ');
                if (note_start) create_item(arg1, note_start + 1);
            }
        } else if (strcmp(cmd, "LABEL") == 0 && matched >= 3) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && matched >= 3) {
            unlabel_item(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}