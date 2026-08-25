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

Item *head = NULL;

Item *find_item(const char *name) {
    for (Item *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) return curr;
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = head;
    head = i;
}

void add_label(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    for (Label *curr = i->labels; curr; curr = curr->next) {
        if (strcmp(curr->name, label_name) == 0) return;
    }
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->name = strdup(label_name);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *curr = i->labels;
    Label *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, label_name) == 0) {
            Label *next = curr->next;
            free(curr->name);
            free(curr);
            if (prev) prev->next = next;
            else i->labels = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_item(const char *name) {
    Item *curr = head;
    Item *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Item *next = curr->next;
            Label *l = curr->labels;
            while (l) {
                Label *ln = l->next;
                free(l->name);
                free(l);
                l = ln;
            }
            free(curr->name);
            free(curr->note);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) return;
    printf("%s %s\n", i->name, i->note);
    for (Label *l = i->labels; l; l = l->next) {
        printf("%s\n", l->name);
    }
}

void report_items() {
    for (Item *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->name, curr->note);
        for (Label *l = curr->labels; l; l = l->next) {
            printf("%s\n", l->name);
        }
    }
}

void free_all() {
    Item *curr = head;
    while (curr) {
        Item *next = curr->next;
        Label *l = curr->labels;
        while (l) {
            Label *ln = l->next;
            free(l->name);
            free(l);
            l = ln;
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
        
        char cmd[16];
        char arg1[1024], arg2[1024];
        
        int n = sscanf(line, "%15s %1023s %1023[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "ITEM") == 0 && n == 3) {
            create_item(arg1, arg2);
        } else if (strcmp(cmd, "LABEL") == 0 && n == 3) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n == 3) {
            unlabel_item(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_item_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}