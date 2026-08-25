// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *name; char *note; char **labels; size_t lc; size_t lcap; } Item;
Item *items = NULL;
size_t ic = 0, icap = 0;

Item* find_item(const char *name) {
    for (size_t i = 0; i < ic; i++) if (strcmp(items[i].name, name) == 0) return &items[i];
    return NULL;
}

void add_item(const char *name, const char *note) {
    if (ic == icap) {
        size_t nc = icap ? icap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Item)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Item *tmp = realloc(items, nc * sizeof(Item));
        if (!tmp) { perror("realloc"); exit(1); }
        items = tmp;
        icap = nc;
    }
    items[ic].name = strdup(name);
    items[ic].note = strdup(note);
    items[ic].labels = NULL;
    items[ic].lc = 0;
    items[ic].lcap = 0;
    if (!items[ic].name || !items[ic].note) { perror("strdup"); exit(1); }
    ic++;
}

void add_label(Item *it, const char *label) {
    if (it->lc == it->lcap) {
        size_t nc = it->lcap ? it->lcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(char*)) { fprintf(stderr, "Overflow\n"); exit(1); }
        char **tmp = realloc(it->labels, nc * sizeof(char*));
        if (!tmp) { perror("realloc"); exit(1); }
        it->labels = tmp;
        it->lcap = nc;
    }
    it->labels[it->lc] = strdup(label);
    if (!it->labels[it->lc]) { perror("strdup"); exit(1); }
    it->lc++;
}

void unlabel(Item *it, const char *label) {
    for (size_t i = 0; i < it->lc; i++) {
        if (strcmp(it->labels[i], label) == 0) {
            free(it->labels[i]);
            it->labels[i] = it->labels[--it->lc];
            return;
        }
    }
}

void del_item(const char *name) {
    for (size_t i = 0; i < ic; i++) {
        if (strcmp(items[i].name, name) == 0) {
            free(items[i].name); free(items[i].note);
            for (size_t j = 0; j < items[i].lc; j++) free(items[i].labels[j]);
            free(items[i].labels);
            items[i] = items[--ic];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ITEM") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_item(name, note);
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) {
                Item *it = find_item(name);
                if (it) add_label(it, label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[256], label[256];
            if (sscanf(line, "%*s %255s %255s", name, label) == 2) {
                Item *it = find_item(name);
                if (it) unlabel(it, label);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_item(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                Item *it = find_item(name);
                if (it) {
                    printf("%s %s", it->name, it->note);
                    for (size_t i = 0; i < it->lc; i++) printf(" %s", it->labels[i]);
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < ic; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (size_t j = 0; j < items[i].lc; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < ic; i++) {
        free(items[i].name); free(items[i].note);
        for (size_t j = 0; j < items[i].lc; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}