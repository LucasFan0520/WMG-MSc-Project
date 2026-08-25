// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct label_node {
    char *label;
    struct label_node *next;
};

struct item {
    char *name;
    char *note;
    struct label_node *labels;
    struct item *next;
};

struct item *items = NULL;

struct item *find_item(char *name) {
    struct item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void create_item(char *name, char *note) {
    if (find_item(name)) return;
    struct item *i = malloc(sizeof(struct item));
    i->name = malloc(strlen(name) + 1);
    strcpy(i->name, name);
    i->note = malloc(strlen(note) + 1);
    strcpy(i->note, note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(char *name, char *label) {
    struct item *i = find_item(name);
    if (!i) return;
    struct label_node *ln = i->labels;
    while (ln) {
        if (strcmp(ln->label, label) == 0) return;
        ln = ln->next;
    }
    ln = malloc(sizeof(struct label_node));
    ln->label = malloc(strlen(label) + 1);
    strcpy(ln->label, label);
    ln->next = i->labels;
    i->labels = ln;
}

void unlabel(char *name, char *label) {
    struct item *i = find_item(name);
    if (!i) return;
    struct label_node *ln = i->labels, *prev = NULL;
    while (ln) {
        if (strcmp(ln->label, label) == 0) {
            if (prev) prev->next = ln->next;
            else i->labels = ln->next;
            free(ln->label);
            free(ln);
            return;
        }
        prev = ln;
        ln = ln->next;
    }
}

void delete_item(char *name) {
    struct item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            struct label_node *ln = i->labels;
            while (ln) {
                struct label_node *next = ln->next;
                free(ln->label);
                free(ln);
                ln = next;
            }
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_print(char *name) {
    struct item *i = find_item(name);
    if (i) {
        printf("%s %s", i->name, i->note);
        struct label_node *ln = i->labels;
        while (ln) {
            printf(" %s", ln->label);
            ln = ln->next;
        }
        printf("\n");
    }
}

void report_items() {
    struct item *i = items;
    while (i) {
        printf("%s %s", i->name, i->note);
        struct label_node *ln = i->labels;
        while (ln) {
            printf(" %s", ln->label);
            ln = ln->next;
        }
        printf("\n");
        i = i->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], arg2[256], arg3[4096];
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, name, arg2, arg3);
        if (strcmp(cmd, "ITEM") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            create_item(name, rest);
        } else if (strcmp(cmd, "LABEL") == 0 && n >= 3) {
            add_label(name, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n >= 3) {
            unlabel(name, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}