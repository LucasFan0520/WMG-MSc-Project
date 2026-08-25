// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct label {
    char *text;
    struct label *next;
} Label;

typedef struct item {
    char *name;
    char *note;
    Label *labels;
    struct item *next;
} Item;

Item *items = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Item *find_item(const char *name) {
    Item *it = items;
    while (it) {
        if (strcmp(it->name, name) == 0) return it;
        it = it->next;
    }
    return NULL;
}

void add_label(Item *it, const char *label) {
    Label *l = malloc(sizeof(Label));
    if (!l) { fprintf(stderr, "Memory error\n"); exit(1); }
    l->text = strdup(label);
    if (!l->text) { fprintf(stderr, "Memory error\n"); exit(1); }
    l->next = it->labels;
    it->labels = l;
}

void item_create(const char *name, const char *note) {
    if (find_item(name)) return;
    Item *it = malloc(sizeof(Item));
    if (!it) { fprintf(stderr, "Memory error\n"); exit(1); }
    it->name = strdup(name);
    it->note = strdup(note);
    if (!it->name || !it->note) { fprintf(stderr, "Memory error\n"); exit(1); }
    it->labels = NULL;
    it->next = items;
    items = it;
}

void label_item(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    add_label(it, label);
}

void unlabel_item(const char *name, const char *label) {
    Item *it = find_item(name);
    if (!it) return;
    Label *l = it->labels, *prev = NULL;
    while (l) {
        if (strcmp(l->text, label) == 0) {
            if (prev) prev->next = l->next;
            else it->labels = l->next;
            free(l->text);
            free(l);
            return;
        }
        prev = l;
        l = l->next;
    }
}

void delete_item(const char *name) {
    Item *it = items, *prev = NULL;
    while (it) {
        if (strcmp(it->name, name) == 0) {
            if (prev) prev->next = it->next;
            else items = it->next;
            free(it->name);
            free(it->note);
            Label *l = it->labels;
            while (l) {
                Label *next = l->next;
                free(l->text);
                free(l);
                l = next;
            }
            free(it);
            return;
        }
        prev = it;
        it = it->next;
    }
}

void find_item_print(const char *name) {
    Item *it = find_item(name);
    if (!it) return;
    for (char *s = it->note; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    printf(" ");
    Label *l = it->labels;
    int first = 1;
    while (l) {
        if (!first) printf(" ");
        printf("%s", l->text);
        first = 0;
        l = l->next;
    }
    printf("\n");
}

void report_items(void) {
    Item *it = items;
    while (it) {
        printf("%s ", it->name);
        for (char *s = it->note; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        printf(" ");
        Label *l = it->labels;
        int first = 1;
        while (l) {
            if (!first) printf(" ");
            printf("%s", l->text);
            first = 0;
            l = l->next;
        }
        printf("\n");
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
            Label *nextl = l->next;
            free(l->text);
            free(l);
            l = nextl;
        }
        free(it);
        it = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report_items(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            item_create(name, rest);
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "LABEL") == 0) label_item(name, rest);
            else unlabel_item(name, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_item_print(rest);
        }
    }
    free_all();
    return 0;
}