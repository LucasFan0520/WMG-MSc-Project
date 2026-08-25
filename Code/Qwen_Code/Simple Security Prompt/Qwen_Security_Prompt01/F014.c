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

Item *head = NULL;

Item *find_item(const char *name) {
    for (Item *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    Item *p = malloc(sizeof(Item));
    p->name = strdup(name);
    p->note = strdup(note);
    p->labels = NULL;
    p->next = head;
    head = p;
}

void add_label(const char *name, const char *label) {
    Item *p = find_item(name);
    if (!p) {
        return;
    }
    Label *l = malloc(sizeof(Label));
    l->name = strdup(label);
    l->next = p->labels;
    p->labels = l;
}

void unlabel_item(const char *name, const char *label) {
    Item *p = find_item(name);
    if (!p) {
        return;
    }
    Label *l = p->labels;
    Label *prev = NULL;
    while (l) {
        if (strcmp(l->name, label) == 0) {
            Label *next = l->next;
            free(l->name);
            free(l);
            if (prev) {
                prev->next = next;
            } else {
                p->labels = next;
            }
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *p = head;
    Item *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            Item *next = p->next;
            Label *l = p->labels;
            while (l) {
                Label *ln = l->next;
                free(l->name);
                free(l);
                l = ln;
            }
            free(p->name);
            free(p->note);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void find_item_print(const char *name) {
    Item *p = find_item(name);
    if (!p) {
        return;
    }
    printf("%s %s", p->name, p->note);
    for (Label *l = p->labels; l; l = l->next) {
        printf(" %s", l->name);
    }
    printf("\n");
}

void report_items() {
    for (Item *p = head; p; p = p->next) {
        printf("%s %s", p->name, p->note);
        for (Label *l = p->labels; l; l = l->next) {
            printf(" %s", l->name);
        }
        printf("\n");
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[8000];
        b[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "ITEM") == 0) {
            create_item(a, b);
        } else if (strcmp(cmd, "LABEL") == 0) {
            add_label(a, b);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            unlabel_item(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_print(a);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    while (head) {
        Item *n = head->next;
        Label *l = head->labels;
        while (l) {
            Label *ln = l->next;
            free(l->name);
            free(l);
            l = ln;
        }
        free(head->name);
        free(head->note);
        free(head);
        head = n;
    }
    return 0;
}