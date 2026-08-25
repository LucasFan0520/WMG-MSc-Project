// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_NOTE 4096
#define MAX_LABEL 64

typedef struct Label {
    char label[MAX_LABEL];
    struct Label *next;
} Label;

typedef struct Item {
    char name[MAX_NAME];
    char note[MAX_NOTE];
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

void free_labels(Label *l) {
    while (l) {
        Label *tmp = l;
        l = l->next;
        free(tmp);
    }
}

void free_all() {
    Item *i = head;
    while (i) {
        Item *tmp = i;
        free_labels(i->labels);
        i = i->next;
        free(tmp);
    }
    head = NULL;
}

void handle_item(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int n = sscanf(line, "ITEM %63s %4095[^\n]", name, note);
    if (n < 2) return;
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    strcpy(i->name, name);
    strcpy(i->note, note);
    i->labels = NULL;
    i->next = head;
    head = i;
}

void handle_label(char *line) {
    char name[MAX_NAME], label[MAX_LABEL];
    if (sscanf(line, "LABEL %63s %63s", name, label) != 2) return;
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels;
    while (l) {
        if (strcmp(l->label, label) == 0) return;
        l = l->next;
    }
    Label *nl = malloc(sizeof(Label));
    if (!nl) return;
    strcpy(nl->label, label);
    nl->next = i->labels;
    i->labels = nl;
}

void handle_unlabel(char *line) {
    char name[MAX_NAME], label[MAX_LABEL];
    if (sscanf(line, "UNLABEL %63s %63s", name, label) != 2) return;
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
    while (l) {
        if (strcmp(l->label, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Item *i = head, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            free_labels(i->labels);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void handle_find(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "FIND %63s", name) != 1) return;
    Item *i = find_item(name);
    if (!i) return;
    printf("%s %s", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf(" %s", l->label);
        l = l->next;
    }
    putchar('\n');
}

void handle_report() {
    Item *i = head;
    while (i) {
        printf("%s %s", i->name, i->note);
        Label *l = i->labels;
        while (l) {
            printf(" %s", l->label);
            l = l->next;
        }
        putchar('\n');
        i = i->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ITEM ", 5) == 0) handle_item(line);
        else if (strncmp(line, "LABEL ", 6) == 0) handle_label(line);
        else if (strncmp(line, "UNLABEL ", 8) == 0) handle_unlabel(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}