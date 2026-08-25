/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct label {
    char *name;
    struct label *next;
} label_t;

typedef struct item {
    char *name;
    char *note;
    label_t *labels;
    struct item *next;
} item_t;

item_t *items = NULL;

item_t *find_item(const char *name) {
    item_t *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void item_cmd(const char *name, const char *note) {
    if (find_item(name)) return;
    item_t *i = malloc(sizeof(item_t));
    if (!i) return;
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
    if (!i->name || !i->note) {
        free(i->name); free(i->note); free(i);
    }
}

void label_cmd(const char *name, const char *label) {
    item_t *i = find_item(name);
    if (!i) return;
    label_t *l = malloc(sizeof(label_t));
    if (!l) return;
    l->name = strdup(label);
    l->next = i->labels;
    i->labels = l;
    if (!l->name) { free(l); }
}

void unlabel_cmd(const char *name, const char *label) {
    item_t *i = find_item(name);
    if (!i) return;
    label_t *prev = NULL, *l = i->labels;
    while (l) {
        if (strcmp(l->name, label) == 0) {
            if (prev) prev->next = l->next;
            else i->labels = l->next;
            free(l->name);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    item_t *prev = NULL, *i = items;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else items = i->next;
            free(i->name);
            free(i->note);
            label_t *l = i->labels;
            while (l) {
                label_t *nxt = l->next;
                free(l->name);
                free(l);
                l = nxt;
            }
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void find_item_print(const char *name) {
    item_t *i = find_item(name);
    if (!i) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s %s", i->name, i->note);
    label_t *l = i->labels;
    while (l) {
        printf(" %s", l->name);
        l = l->next;
    }
    putchar('\n');
}

void report_items(void) {
    item_t *i = items;
    while (i) {
        printf("%s %s", i->name, i->note);
        label_t *l = i->labels;
        while (l) {
            printf(" %s", l->name);
            l = l->next;
        }
        putchar('\n');
        i = i->next;
    }
}

void free_all(void) {
    item_t *i = items;
    while (i) {
        item_t *nxt = i->next;
        free(i->name);
        free(i->note);
        label_t *l = i->labels;
        while (l) {
            label_t *n = l->next;
            free(l->name);
            free(l);
            l = n;
        }
        free(i);
        i = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ITEM") == 0) {
                item_cmd(a, rest);
            } else if (strcmp(cmd, "LABEL") == 0) {
                label_cmd(a, rest);
            } else if (strcmp(cmd, "UNLABEL") == 0) {
                unlabel_cmd(a, rest);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_item(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_item_print(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_items();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}