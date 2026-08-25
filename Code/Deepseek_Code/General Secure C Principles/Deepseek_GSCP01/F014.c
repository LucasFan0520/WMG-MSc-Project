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

Item *items = NULL;

Item *find_item(const char *name) {
    Item *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

Label *find_label(Item *item, const char *label) {
    Label *l = item->labels;
    while (l) {
        if (strcmp(l->label, label) == 0) return l;
        l = l->next;
    }
    return NULL;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void label_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    if (find_label(i, label)) return;
    Label *l = malloc(sizeof(Label));
    l->label = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels, *prev = NULL;
    while (l && strcmp(l->label, label) != 0) {
        prev = l;
        l = l->next;
    }
    if (!l) return;
    if (prev) prev->next = l->next;
    else i->labels = l->next;
    free(l->label);
    free(l);
}

void delete_item(const char *name) {
    Item *i = items, *prev = NULL;
    while (i && strcmp(i->name, name) != 0) {
        prev = i;
        i = i->next;
    }
    if (!i) return;
    if (prev) prev->next = i->next;
    else items = i->next;
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
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) { printf("NOT_FOUND\n"); return; }
    printf("%s ", i->name);
    for (char *c = i->note; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    printf(" ");
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

void report_all(void) {
    Item *i = items;
    while (i) {
        printf("%s ", i->name);
        for (char *c = i->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        printf(" ");
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

void free_all(void) {
    Item *i = items;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i->note);
        Label *l = i->labels;
        while (l) {
            Label *nextl = l->next;
            free(l->label);
            free(l);
            l = nextl;
        }
        free(i);
        i = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            char *p = line + 5;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            item_create(name, p);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line + 6, "%255s %255s", name, label) == 2)
                label_item(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line + 8, "%255s %255s", name, label) == 2)
                unlabel_item(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_item(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_item_print(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}