// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *store = NULL;

Note* find_note(const char *name) {
    Note *cur = store;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_note(const char *name, const char *note) {
    if (find_note(name)) return;
    Note *new = malloc(sizeof(Note));
    new->name = strdup(name);
    new->note = strdup(note);
    new->next = NULL;
    if (!store) store = new;
    else {
        Note *cur = store;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_note(const char *name) {
    Note *prev = NULL, *cur = store;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else store = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report_notes() {
    Note *cur = store;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void free_store(Note *list) {
    Note *cur = list;
    while (cur) {
        Note *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int load_file(const char *path, Note **new_store) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[4096];
    Note *head = NULL;
    Note *tail = NULL;
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, " ");
        if (!name) { valid = 0; break; }
        char *note = strtok(NULL, "");
        if (!note) note = "";
        Note *new = malloc(sizeof(Note));
        new->name = strdup(name);
        new->note = strdup(note);
        new->next = NULL;
        if (!head) head = new;
        else tail->next = new;
        tail = new;
    }
    fclose(f);
    if (valid) {
        *new_store = head;
        return 1;
    } else {
        // free partial list
        free_store(head);
        return 0;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        Note *loaded = NULL;
        if (load_file(argv[1], &loaded)) {
            // replace current store
            free_store(store);
            store = loaded;
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            // LOAD without filename? The spec says it receives file path as first argument, so LOAD command may not take an argument.
            // We'll assume LOAD uses the file path given at startup.
            if (argc > 1) {
                Note *loaded = NULL;
                if (load_file(argv[1], &loaded)) {
                    free_store(store);
                    store = loaded;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_store(store);
    return 0;
}