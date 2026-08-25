// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Item* find_item(const char *name) {
    for (Item *i = items; i; i = i->next)
        if (strcmp(i->name, name) == 0) return i;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            if (find_item(name)) continue;
            Item *i = malloc(sizeof(Item));
            if (!i) continue;
            i->name = strdup(name);
            i->note = strdup(note);
            i->labels = NULL;
            i->next = items;
            items = i;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                Label *l = malloc(sizeof(Label));
                if (!l) continue;
                l->name = strdup(label);
                l->next = i->labels;
                i->labels = l;
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(rest, "%255s %255s", name, label) != 2) continue;
            Item *i = find_item(name);
            if (i) {
                Label *prev = NULL;
                for (Label *l = i->labels; l; prev = l, l = l->next) {
                    if (strcmp(l->name, label) == 0) {
                        if (prev) prev->next = l->next;
                        else i->labels = l->next;
                        free(l->name);
                        free(l);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Item *prev = NULL;
            for (Item *i = items; i; prev = i, i = i->next) {
                if (strcmp(i->name, name) == 0) {
                    if (prev) prev->next = i->next;
                    else items = i->next;
                    while (i->labels) {
                        Label *l = i->labels;
                        i->labels = l->next;
                        free(l->name);
                        free(l);
                    }
                    free(i->name);
                    free(i->note);
                    free(i);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Item *i = find_item(name);
            if (i) {
                printf("%s ", i->name);
                print_text(i->note);
                for (Label *l = i->labels; l; l = l->next) printf("%s\n", l->name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = items; i; i = i->next) {
                printf("%s ", i->name);
                print_text(i->note);
                for (Label *l = i->labels; l; l = l->next) printf("%s\n", l->name);
            }
        }
    }
    while (items) {
        Item *i = items;
        items = items->next;
        while (i->labels) {
            Label *l = i->labels;
            i->labels = l->next;
            free(l->name);
            free(l);
        }
        free(i->name);
        free(i->note);
        free(i);
    }
    return 0;
}