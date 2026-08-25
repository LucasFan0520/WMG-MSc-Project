// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *store = NULL;  // current note store

void free_notes(Note *n) {
    while (n) {
        Note *tmp = n;
        n = n->next;
        free(tmp->name);
        free(tmp->text);
        free(tmp);
    }
}

int load_notes(const char *filename, Note **new_store) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    char line[4096];
    Note *head = NULL, *tail = NULL;
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        // Expect format: name note (first space separates name and note)
        char *space = strchr(line, ' ');
        if (!space) { valid = 0; break; }
        *space = '\0';
        char *name = line;
        char *text = space + 1;
        Note *new = malloc(sizeof(Note));
        new->name = strdup(name);
        new->text = strdup(text);
        new->next = NULL;
        if (!head) head = tail = new;
        else {
            tail->next = new;
            tail = new;
        }
    }
    fclose(f);
    if (!valid) {
        free_notes(head);
        return 0;
    }
    *new_store = head;
    return 1;
}

void handle_load(char *cmd) {
    // first argument is filename, stored globally? We'll need to know it.
    // We'll store filename as static.
    static char *filename = NULL;
    // Actually we need to know the file path from command line.
    // We'll set it in main and use a global.
    // For simplicity, we'll use a global variable.
    // We'll define a global char *config_file.
    // But we can also just pass it via main.
    // We'll implement with a static variable that main sets.
}

// We'll use a global to hold the filename.
static char *config_file = NULL;

void handle_load_cmd() {
    if (!config_file) return;
    Note *new_store = NULL;
    if (load_notes(config_file, &new_store)) {
        free_notes(store);
        store = new_store;
    }
}

void handle_add(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *text = strtok(NULL, "");
    if (!text) text = "";
    Note *new = malloc(sizeof(Note));
    new->name = strdup(name);
    new->text = strdup(text);
    new->next = NULL;
    if (!store) store = new;
    else {
        Note *last = store;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_delete(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
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

void handle_report() {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

int main(int argc, char **argv) {
    if (argc == 2) {
        config_file = strdup(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "LOAD") == 0) {
            handle_load_cmd();
        } else if (strcmp(cmd, "ADD") == 0) {
            handle_add(cmd);
        } else if (strcmp(cmd, "DELETE") == 0) {
            handle_delete(cmd);
        } else if (strcmp(cmd, "REPORT") == 0) {
            handle_report();
        }
    }
    free_notes(store);
    free(config_file);
    return 0;
}