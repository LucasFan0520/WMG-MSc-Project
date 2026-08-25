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
    Item *cur = items;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *it = malloc(sizeof(Item));
    it->name = strdup(name);
    it->note = strdup(note);
    it->labels = NULL;
    it->next = items;
    items = it;
}

void add_label(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    Label *l = malloc(sizeof(Label));
    l->label = strdup(label);
    l->next = it->labels;
    it->labels = l;
}

void unlabel(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    Label *cur = it->labels, *prev = NULL;
    while (cur) {
        if (strcmp(cur->label, label) == 0) {
            if (prev) prev->next = cur->next;
            else it->labels = cur->next;
            free(cur->label);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_item(const char *name) {
    Item *it = find_item(name);
    if (!it) return;
    Item *cur = items, *prev = NULL;
    while (cur) {
        if (cur == it) {
            if (prev) prev->next = cur->next;
            else items = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(it->name);
    free(it->note);
    Label *l = it->labels;
    while (l) {
        Label *tmp = l;
        l = l->next;
        free(tmp->label);
        free(tmp);
    }
    free(it);
}

void find_item_cmd(const char *name) {
    Item *it = find_item(name);
    if (!it) return;
    printf("%s %s", it->name, it->note);
    Label *l = it->labels;
    while (l) {
        printf(" %s", l->label);
        l = l->next;
    }
    putchar('\n');
}

void report_items(void) {
    Item *it = items;
    while (it) {
        printf("%s %s", it->name, it->note);
        Label *l = it->labels;
        while (l) {
            printf(" %s", l->label);
            l = l->next;
        }
        putchar('\n');
        it = it->next;
    }
}

void cleanup_items(void) {
    Item *it = items;
    while (it) {
        Item *tmp = it;
        it = it->next;
        free(tmp->name);
        free(tmp->note);
        Label *l = tmp->labels;
        while (l) {
            Label *tl = l;
            l = l->next;
            free(tl->label);
            free(tl);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        int n = sscanf(line, "%31s %255s %1023s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ITEM") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_item(arg1, note);
        } else if (strcmp(cmd, "LABEL") == 0 && n == 3) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n == 3) {
            unlabel(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    cleanup_items();
    return 0;
}