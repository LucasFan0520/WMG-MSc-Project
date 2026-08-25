/* F014.c */
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

Item *items = NULL;

Item *find_item(const char *name) {
    Item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(Item *i, const char *label) {
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->name = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void label_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    add_label(i, label);
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
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
    Item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            while (i->labels) {
                Label *l = i->labels;
                i->labels = i->labels->next;
                free(l->name);
                free(l);
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
    if (!i) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s\n", i->name, i->note);
    Label *l = i->labels;
    while (l) {
        printf("%s\n", l->name);
        l = l->next;
    }
}

void report_all(void) {
    Item *i = items;
    while (i) {
        printf("%s %s\n", i->name, i->note);
        Label *l = i->labels;
        while (l) {
            printf("%s\n", l->name);
            l = l->next;
        }
        i = i->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + 2;
            while (*note == ' ') note++;
            item_create(arg1, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            label_item(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            unlabel_item(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (items) {
        Item *tmp = items;
        items = items->next;
        free(tmp->name);
        free(tmp->note);
        while (tmp->labels) {
            Label *l = tmp->labels;
            tmp->labels = tmp->labels->next;
            free(l->name);
            free(l);
        }
        free(tmp);
    }
    return 0;
}