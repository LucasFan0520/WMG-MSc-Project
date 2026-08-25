/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_labels(Label *l) {
    while (l) {
        Label *tmp = l;
        l = l->next;
        free(tmp->name);
        free(tmp);
    }
}

void free_item(Item *i) {
    free(i->name);
    free(i->note);
    free_labels(i->labels);
    free(i);
}

void item_create(const char *name, const char *note) {
    Item *i = find_item(name);
    if (i) {
        free(i->note);
        i->note = strdup(note);
        return;
    }
    i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void label_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->name = strdup(label);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *name, const char *label) {
    Item *i = find_item(name);
    if (!i) return;
    Label *prev = NULL, *curr = i->labels;
    while (curr) {
        if (strcmp(curr->name, label) == 0) {
            if (prev) prev->next = curr->next;
            else i->labels = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void delete_item(const char *name) {
    Item *prev = NULL, *curr = items;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else items = curr->next;
            free_item(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_item_print(const char *name) {
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
    putchar('\n');
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
        putchar('\n');
        i = i->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], label[256], note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ITEM") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            item_create(name, p);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, label) != 2) continue;
            label_item(name, label);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, label) != 2) continue;
            unlabel_item(name, label);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) find_item_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }

    while (items) {
        Item *tmp = items;
        items = items->next;
        free_item(tmp);
    }
    return 0;
}