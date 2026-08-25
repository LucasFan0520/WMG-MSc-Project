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

Item *head = NULL;

Item* find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void free_labels(LabelNode *l) {
    while (l) {
        LabelNode *tmp = l;
        l = l->next;
        free(tmp->label);
        free(tmp);
    }
}

void free_item(Item *i) {
    free(i->name);
    free(i->note);
    free_labels(i->labels);
    free(i);
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *new = malloc(sizeof(Item));
    new->name = strdup(name);
    new->note = strdup(note);
    new->labels = NULL;
    new->next = NULL;
    if (!head) head = new;
    else {
        Item *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void label_add(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    // Avoid duplicate label? Not specified, but we'll allow duplicates? Usually not.
    LabelNode *l = i->labels;
    while (l) {
        if (strcmp(l->label, label) == 0) return; // duplicate
        l = l->next;
    }
    LabelNode *new = malloc(sizeof(LabelNode));
    new->label = strdup(label);
    new->next = NULL;
    if (!i->labels) i->labels = new;
    else {
        LabelNode *last = i->labels;
        while (last->next) last = last->next;
        last->next = new;
    }
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
            free_item(i);
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
    putchar('\n');
}

void report_all() {
    Item *i = head;
    while (i) {
        printf("%s %s", i->name, i->note);
        LabelNode *l = i->labels;
        while (l) {
            printf(" %s", l->label);
            l = l->next;
        }
        putchar('\n');
        i = i->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name) {
                if (!note) note = "";
                item_create(name, note);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (name && label) label_add(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (name && label) unlabel(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (head) {
        Item *tmp = head;
        head = head->next;
        free_item(tmp);
    }
    return 0;
}