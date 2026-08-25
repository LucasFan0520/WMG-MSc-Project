// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

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
    for (Item *i = items; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

void create_item(const char *name, const char *note) {
    Item *i = malloc(sizeof(Item));
    i->name = strdup(name);
    i->note = strdup(note);
    i->labels = NULL;
    i->next = items;
    items = i;
}

void add_label(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *l = malloc(sizeof(Label));
    l->name = strdup(label_name);
    l->next = i->labels;
    i->labels = l;
}

void unlabel_item(const char *item_name, const char *label_name) {
    Item *i = find_item(item_name);
    if (!i) return;
    Label *prev = NULL, *curr = i->labels;
    while (curr) {
        if (strcmp(curr->name, label_name) == 0) {
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
            Label *l = curr->labels;
            while (l) {
                Label *next = l->next;
                free(l->name);
                free(l);
                l = next;
            }
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void find_print_item(const char *name) {
    Item *i = find_item(name);
    if (i) {
        printf("%s: %s\n", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) {
            printf("  %s\n", l->name);
        }
    }
}

void report_items() {
    for (Item *i = items; i; i = i->next) {
        printf("%s: %s\n", i->name, i->note);
        for (Label *l = i->labels; l; l = l->next) {
            printf("  %s\n", l->name);
        }
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        char *note = strchr(line, ' ');
        if (note) {
            note++;
            while (*note == ' ') note++;
            note = strchr(note, ' ');
            if (note) {
                note++;
                while (*note == ' ') note++;
            } else {
                note = NULL;
            }
        }
        
        if (strcmp(cmd, "ITEM") == 0 && note) {
            create_item(arg1, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            add_label(arg1, arg2);
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            unlabel_item(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_item(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_print_item(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
        free(line);
    }
    
    Item *i = items;
    while (i) {
        Item *next = i->next;
        Label *l = i->labels;
        while (l) {
            Label *next_l = l->next;
            free(l->name);
            free(l);
            l = next_l;
        }
        free(i->name);
        free(i->note);
        free(i);
        i = next;
    }
    return 0;
}