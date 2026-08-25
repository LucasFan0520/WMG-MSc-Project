// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label { char *text; struct Label *next; } Label;
typedef struct Item { char *name, *note; Label *labels; struct Item *next; } Item;
Item *head = NULL;

void add_item(const char *name, const char *note) {
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = head;
    head = i;
}

void add_label(const char *name, const char *label) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) {
            Label *l = malloc(sizeof(Label));
            l->text = strdup(label);
            l->next = i->labels;
            i->labels = l;
            return;
        }
    }
}

void unlabel(const char *name, const char *label) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) {
            Label **p = &i->labels;
            while (*p) {
                if (strcmp((*p)->text, label) == 0) {
                    Label *tmp = *p;
                    *p = tmp->next;
                    free(tmp->text);
                    free(tmp);
                    return;
                }
                p = &(*p)->next;
            }
        }
    }
}

void delete_item(const char *name) {
    Item **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Item *tmp = *p;
            *p = tmp->next;
            while (tmp->labels) {
                Label *l = tmp->labels->next;
                free(tmp->labels->text);
                free(tmp->labels);
                tmp->labels = l;
            }
            free(tmp->name);
            free(tmp->note);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void find_item(const char *name) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) {
            printf("%s %s", i->name, i->note);
            for (Label *l = i->labels; l; l = l->next) printf(" %s", l->text);
            printf("\n");
            return;
        }
    }
}

void report() {
    for (Item *i = head; i; i = i->next) {
        printf("%s %s", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) printf(" %s", l->text);
        printf("\n");
    }
}

void cleanup() {
    while (head) {
        Item *tmp = head->next;
        while (head->labels) {
            Label *l = head->labels->next;
            free(head->labels->text);
            free(head->labels);
            head->labels = l;
        }
        free(head->name);
        free(head->note);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_item(a1, t + 1); }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) add_label(a1, a2);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) unlabel(a1, a2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_item(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_item(a1);
        } else if (strcmp(cmd, "REPORT") == 0) report();
    }
    cleanup();
    return 0;
}