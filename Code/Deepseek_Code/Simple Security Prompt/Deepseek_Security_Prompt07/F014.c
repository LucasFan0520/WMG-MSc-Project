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

void add_label_to_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void remove_label_from_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL;
    Label *l = i->labels;
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
    Item *prev = NULL;
    Item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            Label *l = i->labels;
            while (l) {
                Label *next = l->next;
                free(l->label);
                free(l);
                l = next;
            }
            free(i->name);
            free(i->note);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s:", i->name);
    for (char *s = i->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar(' ');
    Label *l = i->labels;
    int first = 1;
    while (l) {
        if (!first) putchar(' ');
        printf("%s", l->label);
        first = 0;
        l = l->next;
    }
    putchar('\n');
}

void report_all_items() {
    Item *i = items;
    while (i) {
        printf("%s:", i->name);
        for (char *s = i->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar(' ');
        Label *l = i->labels;
        int first = 1;
        while (l) {
            if (!first) putchar(' ');
            printf("%s", l->label);
            first = 0;
            l = l->next;
        }
        putchar('\n');
        i = i->next;
    }
}

void free_all() {
    Item *i = items;
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                create_item(name, note);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) == 2) {
                add_label_to_item(name, label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) == 2) {
                remove_label_from_item(name, label);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                delete_item(name);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                find_item_print(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all_items();
        }
    }
    free_all();
    return 0;
}