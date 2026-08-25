// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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
    Item *it = items;
    while (it) {
        if (strcmp(it->name, name) == 0) return it;
        it = it->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void create_item(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *it = malloc(sizeof(Item));
    if (!it) return;
    it->name = safe_strdup(name);
    it->note = safe_strdup(note);
    it->labels = NULL;
    it->next = items;
    items = it;
}

void add_label(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    Label *l = malloc(sizeof(Label));
    if (!l) return;
    l->name = safe_strdup(label);
    l->next = it->labels;
    it->labels = l;
}

void remove_label(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    Label *prev = NULL, *cur = it->labels;
    while (cur) {
        if (strcmp(cur->name, label) == 0) {
            if (prev) prev->next = cur->next;
            else it->labels = cur->next;
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
            free(cur->name);
            free(cur->note);
            Label *l = cur->labels;
            while (l) {
                Label *next = l->next;
                free(l->name);
                free(l);
                l = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_item_cmd(const char *name) {
    Item *it = find_item(name);
    if (!it) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s", it->name, it->note);
    Label *l = it->labels;
    while (l) {
        printf(" %s", l->name);
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
            printf(" %s", l->name);
            l = l->next;
        }
        putchar('\n');
        it = it->next;
    }
}

void free_all(void) {
    Item *it = items;
    while (it) {
        Item *next = it->next;
        free(it->name);
        free(it->note);
        Label *l = it->labels;
        while (l) {
            Label *next_l = l->next;
            free(l->name);
            free(l);
            l = next_l;
        }
        free(it);
        it = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ITEM") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            create_item(arg1, p);
        } else if (strcmp(cmd, "LABEL") == 0 && n >= 3) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0 && n >= 3) {
            remove_label(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_item_cmd(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}