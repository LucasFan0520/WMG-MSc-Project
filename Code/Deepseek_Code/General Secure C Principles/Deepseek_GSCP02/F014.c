// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    char *text;
    struct Label *next;
} Label;

typedef struct Item {
    char *name;
    char *note;
    Label *l_head;
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

void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->l_head = NULL;
    i->next = head;
    head = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->text = strdup(label);
    l->next = i->l_head;
    i->l_head = l;
}

void remove_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL;
    Label *l = i->l_head;
    while (l) {
        if (strcmp(l->text, label) == 0) {
            if (prev) prev->next = l->next;
            else i->l_head = l->next;
            free(l->text);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL;
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            free(i->name);
            free(i->note);
            Label *l = i->l_head;
            while (l) {
                Label *next = l->next;
                free(l->text);
                free(l);
                l = next;
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
    printf("%s ", i->name);
    for (char *p = i->note; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
    }
    putchar('\n');
    Label *l = i->l_head;
    while (l) {
        for (char *p = l->text; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        l = l->next;
    }
}

void report_all(void) {
    Item *i = head;
    while (i) {
        printf("%s ", i->name);
        for (char *p = i->note; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        Label *l = i->l_head;
        while (l) {
            for (char *p = l->text; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            l = l->next;
        }
        i = i->next;
    }
}

void free_all(void) {
    while (head) {
        Item *next = head->next;
        free(head->name);
        free(head->note);
        Label *l = head->l_head;
        while (l) {
            Label *nextl = l->next;
            free(l->text);
            free(l);
            l = nextl;
        }
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ITEM") == 0) {
            char name[256], note[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, note) == 2) {
                create_item(name, note);
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) {
                add_label(name, label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) {
                remove_label(name, label);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_item(name);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                find_item_print(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}