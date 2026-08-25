/* F014.c */
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

Item *head = NULL;

Item* find_item(const char *name) {
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
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = head;
    head = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    LabelNode *l = i->labels;
    while (l) {
        if (strcmp(l->label, label) == 0) return;
        l = l->next;
    }
    LabelNode *new = malloc(sizeof(LabelNode));
    new->label = strdup(label);
    new->next = i->labels;
    i->labels = new;
}

void unlabel(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    LabelNode *l = i->labels, *prev = NULL;
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
            LabelNode *l = i->labels;
            while (l) {
                LabelNode *tmp = l;
                l = l->next;
                free(tmp->label);
                free(tmp);
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
    printf("%s %s", i->name, i->note);
    LabelNode *l = i->labels;
    while (l) {
        printf(" %s", l->label);
        l = l->next;
    }
    printf("\n");
}

void report_items() {
    Item *i = head;
    while (i) {
        printf("%s %s", i->name, i->note);
        LabelNode *l = i->labels;
        while (l) {
            printf(" %s", l->label);
            l = l->next;
        }
        printf("\n");
        i = i->next;
    }
}

void free_all() {
    while (head) {
        Item *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        LabelNode *l = tmp->labels;
        while (l) {
            LabelNode *ltmp = l;
            l = l->next;
            free(ltmp->label);
            free(ltmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256]; char rest[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, name, rest);
        if (strcmp(cmd, "ITEM") == 0 && n == 3) {
            create_item(name, rest);
        } else if (strcmp(cmd, "LABEL") == 0 && n == 3) {
            add_label(name, rest);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n == 3) {
            unlabel(name, rest);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_items();
        }
    }
    free_all();
    return 0;
}