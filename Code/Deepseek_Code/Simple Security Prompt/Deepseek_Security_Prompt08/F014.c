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

Item *head = NULL, *tail = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    Item *i = malloc(sizeof(Item));
    i->name = malloc(strlen(name) + 1);
    strcpy(i->name, name);
    i->note = malloc(strlen(note) + 1);
    strcpy(i->note, note);
    i->labels = NULL;
    i->next = NULL;
    if (tail) {
        tail->next = i;
        tail = i;
    } else {
        head = tail = i;
    }
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->label = malloc(strlen(label) + 1);
    strcpy(l->label, label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL, *curr = i->labels;
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
    Item *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Label *l = curr->labels;
            while (l) {
                Label *next = l->next;
                free(l->label);
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
        printf(" %s", l->label);
        l = l->next;
    }
    putchar('\n');
}

void report_items(void) {
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

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *next = i->next;
        Label *l = i->labels;
        while (l) {
            Label *nextl = l->next;
            free(l->label);
            free(l);
            l = nextl;
        }
        free(i->name);
        free(i->note);
        free(i);
        i = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) create_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *label = strtok(NULL, " ");
            if (name && label) add_label(name, label);
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
            report_items();
        }
    }
    free(line);
    free_all();
    return 0;
}