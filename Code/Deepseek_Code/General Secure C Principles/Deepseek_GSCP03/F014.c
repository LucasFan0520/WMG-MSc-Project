/* F014.c */
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
    Label *labels;
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
    i->labels = NULL;
    i->next = head;
    head = i;
}

void add_label(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = i->labels;
    while (l) {
        if (strcmp(l->text, label) == 0) return;
        l = l->next;
    }
    Label *new_l = malloc(sizeof(Label));
    if (!new_l) return;
    new_l->text = strdup(label);
    new_l->next = i->labels;
    i->labels = new_l;
}

void unlabel(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL, *curr = i->labels;
    while (curr) {
        if (strcmp(curr->text, label) == 0) {
            if (prev) prev->next = curr->next;
            else i->labels = curr->next;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            Label *l = curr->labels;
            while (l) {
                Label *tmp = l;
                l = l->next;
                free(tmp->text);
                free(tmp);
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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
        printf(" %s", l->text);
        l = l->next;
    }
    printf("\n");
}

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %s", i->name, i->note);
        Label *l = i->labels;
        while (l) {
            printf(" %s", l->text);
            l = l->next;
        }
        printf("\n");
        i = i->next;
    }
}

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *tmp = i;
        i = i->next;
        free(tmp->name);
        free(tmp->note);
        Label *l = tmp->labels;
        while (l) {
            Label *tmp_l = l;
            l = l->next;
            free(tmp_l->text);
            free(tmp_l);
        }
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    create_item(name, note);
                }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line + 6, "%255s %255s", name, label) == 2) {
                add_label(name, label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line + 8, "%255s %255s", name, label) == 2) {
                unlabel(name, label);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_item(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_item_cmd(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}