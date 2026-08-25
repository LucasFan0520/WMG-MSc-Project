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
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->name = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void remove_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL, *cur = i->labels;
    while (cur) {
        if (strcmp(cur->name, label) == 0) {
            if (prev) prev->next = cur->next;
            else i->labels = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL, *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else items = cur->next;
            Label *l = cur->labels;
            while (l) {
                Label *next = l->next;
                free(l->name);
                free(l);
                l = next;
            }
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_item_print(const char *name) {
    Item *i = find_item(name);
    if (!i) return;
    printf("%s: ", i->name);
    char *s = i->note;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
    Label *l = i->labels;
    while (l) {
        printf("  %s\n", l->name);
        l = l->next;
    }
}

void report_items(void) {
    Item *i = items;
    while (i) {
        printf("%s: ", i->name);
        char *s = i->note;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        Label *l = i->labels;
        while (l) {
            printf("  %s\n", l->name);
            l = l->next;
        }
        i = i->next;
    }
}

void free_all(void) {
    Item *i = items;
    while (i) {
        Item *next = i->next;
        Label *l = i->labels;
        while (l) {
            Label *nextl = l->next;
            free(l->name);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            create_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *label = space + 1;
            while (*label == ' ') label++;
            add_label(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *label = space + 1;
            while (*label == ' ') label++;
            remove_label(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}