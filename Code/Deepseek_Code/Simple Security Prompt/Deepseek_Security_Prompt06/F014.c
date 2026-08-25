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

void add_label(Item *i, const char *label) {
    Label *l = malloc(sizeof(Label));
    l->label = malloc(strlen(label) + 1);
    strcpy(l->label, label);
    l->next = i->labels;
    i->labels = l;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    i->name = malloc(strlen(name) + 1);
    strcpy(i->name, name);
    i->note = malloc(strlen(note) + 1);
    strcpy(i->note, note);
    i->labels = NULL;
    i->next = items;
    items = i;
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
    Item *i = items, *prev = NULL;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            Label *l = i->labels;
            while (l) {
                Label *tmp = l->next;
                free(l->label);
                free(l);
                l = tmp;
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
    Item *i = items;
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
    Item *i = items;
    while (i) {
        Item *tmp = i->next;
        free(i->name);
        free(i->note);
        Label *l = i->labels;
        while (l) {
            Label *tl = l->next;
            free(l->label);
            free(l);
            l = tl;
        }
        free(i);
        i = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], rest[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *ptr = line + strlen(cmd) + 1;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(ptr, "%255s", name) == 1) {
                const char *n = ptr + strlen(name) + 1;
                while (*n == ' ') n++;
                if (*n) item_create(name, n);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(ptr, "%255s %4095s", name, rest) == 2) {
                label_item(name, rest);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(ptr, "%255s %4095s", name, rest) == 2) {
                unlabel_item(name, rest);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(ptr, "%255s", name) == 1) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(ptr, "%255s", name) == 1) find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}