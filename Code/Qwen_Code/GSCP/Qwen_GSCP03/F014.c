// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    char *text;
    struct Label *next;
} Label;

typedef struct Item {
    char *name, *note;
    Label *labels;
    struct Item *next;
} Item;

Item *head = NULL;

void free_labels(Label *l) {
    while (l) { Label *t = l; l = l->next; free(t->text); free(t); }
}

void free_list() {
    while (head) {
        Item *t = head; head = head->next;
        free(t->name); free(t->note); free_labels(t->labels); free(t);
    }
}

Item *find_item(const char *name) {
    for (Item *i = head; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], val[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ITEM") == 0) {
            if (sscanf(line, "%*s %255s %255[^\n]", name, val) < 1) continue;
            if (find_item(name)) continue;
            Item *i = malloc(sizeof(Item));
            i->name = strdup(name); i->note = strdup(val); i->labels = NULL;
            i->next = head; head = i;
        } else if (strcmp(cmd, "LABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, val) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                Label *l = malloc(sizeof(Label));
                l->text = strdup(val); l->next = i->labels; i->labels = l;
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, val) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                Label *prev = NULL, *curr = i->labels;
                while (curr) {
                    if (strcmp(curr->text, val) == 0) {
                        Label *tmp = curr; curr = curr->next;
                        if (prev) prev->next = curr; else i->labels = curr;
                        free(tmp->text); free(tmp);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Item *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Item *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->note); free_labels(tmp->labels); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Item *i = find_item(name);
            if (i) {
                printf("%s %s", i->name, i->note);
                for (Label *l = i->labels; l; l = l->next) printf(" %s", l->text);
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = head; i; i = i->next) {
                printf("%s %s", i->name, i->note);
                for (Label *l = i->labels; l; l = l->next) printf(" %s", l->text);
                printf("\n");
            }
        }
    }
    free_list();
    return 0;
}