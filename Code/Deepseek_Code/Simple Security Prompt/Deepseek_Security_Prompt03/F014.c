/*
 * F014.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct label {
    char *name;
    struct label *next;
} Label;

typedef struct item {
    char *name;
    char *note;
    Label *labels;
    struct item *next;
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

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    // avoid duplicate label?
    Label *l = i->labels;
    while (l) {
        if (strcmp(l->name, label) == 0) return;
        l = l->next;
    }
    Label *nl = malloc(sizeof(Label));
    nl->name = strdup(label);
    nl->next = i->labels;
    i->labels = nl;
}

void unlabel(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL, *l = i->labels;
    while (l) {
        if (strcmp(l->name, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->name);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL, *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            Label *l = i->labels;
            while (l) {
                Label *tmp = l;
                l = l->next;
                free(tmp->name);
                free(tmp);
            }
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_cmd(const char *name) {
    Item *i = find_item(name);
    if (!i) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf(" %s", l->name);
        l = l->next;
    }
    printf("\n");
}

void report_items(void) {
    Item *i = items;
    while (i) {
        printf("%s %s", i->name, i->note);
        Label *l = i->labels;
        while (l) {
            printf(" %s", l->name);
            l = l->next;
        }
        printf("\n");
        i = i->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        if (sscanf(line, "%31s %255s", cmd, arg1) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ITEM") == 0) {
            create_item(arg1, rest);
        } else if (strcmp(cmd, "LABEL") == 0) {
            add_label(arg1, rest);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            unlabel(arg1, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    return 0;
}