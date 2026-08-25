/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *name;
    char *text;
    struct note *next;
} Note;

Note *store = NULL;

Note *find_note(const char *name) {
    Note *n = store;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *text) {
    Note *n = find_note(name);
    if (n) {
        free(n->text);
        n->text = strdup(text);
        return;
    }
    Note *new = malloc(sizeof(Note));
    new->name = strdup(name);
    new->text = strdup(text);
    new->next = store;
    store = new;
}

void delete_note(const char *name) {
    Note *n = store, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else store = n->next;
            free(n->name);
            free(n->text);
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void report() {
    Note *n = store;
    while (n) {
        printf("%s ", n->name);
        for (char *c = n->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        n = n->next;
    }
}

void load_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    Note *temp = NULL;
    int valid = 1;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[1024];
        if (sscanf(line, "%255s", name) < 1) { valid = 0; break; }
        char *rest = line + strlen(name) + 1;
        if (*rest == ' ') rest++;
        Note *n = malloc(sizeof(Note));
        n->name = strdup(name);
        n->text = strdup(rest);
        n->next = temp;
        temp = n;
    }
    fclose(f);
    if (!valid) {
        // discard temp
        Note *n = temp;
        while (n) { Note *tmp = n; n = n->next; free(tmp->name); free(tmp->text); free(tmp); }
        return;
    }
    // replace current store
    Note *old = store;
    store = temp;
    // free old store
    Note *n = old;
    while (n) { Note *tmp = n; n = n->next; free(tmp->name); free(tmp->text); free(tmp); }
}

int main(int argc, char **argv) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], text[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_from_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            add_note(name, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_note(name);
        }
    }
    return 0;
}