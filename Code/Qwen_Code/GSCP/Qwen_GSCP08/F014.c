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

Item *find_item(char *name) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

void create_item(char *name, char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = head;
    head = i;
}

void add_label(char *item_name, char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->name = strdup(label_name);
    l->next = i->labels;
    i->labels = l;
}

void remove_label(char *item_name, char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *prev = NULL;
    Label *curr = i->labels;
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

void delete_item(char *name) {
    Item *prev = NULL;
    Item *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            Label *l = curr->labels;
            while (l) {
                Label *next = l->next;
                free(l->name);
                free(l);
                l = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_item(char *name) {
    Item *i = find_item(name);
    if (i) {
        printf("%s %s", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) printf(" %s", l->name);
        puts("");
    }
}

void report_items() {
    for (Item *i = head; i; i = i->next) {
        printf("%s %s", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) printf(" %s", l->name);
        puts("");
    }
}

int main() {
    char cmd[20], a[100], b[100], note[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ITEM") == 0) {
            scanf("%99s", a);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            create_item(a, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            scanf("%99s%99s", a, b);
            add_label(a, b);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            scanf("%99s%99s", a, b);
            remove_label(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_item(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            print_item(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}