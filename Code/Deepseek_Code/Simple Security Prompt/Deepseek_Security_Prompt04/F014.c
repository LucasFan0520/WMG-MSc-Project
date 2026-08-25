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

Item *head = NULL;

Item *find_item(char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void create_item(char *name, char *note) {
    Item *i = find_item(name);
    if (i) {
        free(i->note);
        i->note = strdup(note);
        return;
    }
    Item *new = malloc(sizeof(Item));
    new->name = strdup(name);
    new->note = strdup(note);
    new->labels = NULL;
    new->next = head;
    head = new;
}

void add_label(char *name, char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel(char *name, char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL;
    for (Label *l = i->labels; l; l = l->next) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->label);
            free(l);
            return;
        }
        prev = l;
    }
}

void delete_item(char *name) {
    Item *prev = NULL;
    for (Item *i = head; i; i = i->next) {
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
    }
}

void find_item_print(char *name) {
    Item *i = find_item(name);
    if (!i) return;
    printf("%s %s\n", i->name, i->note);
    for (Label *l = i->labels; l; l = l->next)
        printf("  %s\n", l->label);
}

void report_all(void) {
    for (Item *i = head; i; i = i->next) {
        printf("%s %s\n", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next)
            printf("  %s\n", l->label);
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
            Label *lnext = l->next;
            free(l->label);
            free(l);
            l = lnext;
        }
        free(i);
        i = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ITEM") == 0 && n == 3) {
            create_item(arg1, arg2);
        } else if (strcmp(cmd, "LABEL") == 0 && n == 3) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n == 3) {
            unlabel(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_item_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}