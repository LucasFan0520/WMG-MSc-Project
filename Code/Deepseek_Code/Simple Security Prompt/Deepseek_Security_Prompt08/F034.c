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

void free_store(Note *s) {
    Note *curr = s;
    while (curr) {
        Note *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

Note *load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;
    Note *new_head = NULL, *new_tail = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int valid = 1;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        char *name = strtok(line, " ");
        char *note = strtok(NULL, "");
        if (!name || !note) { valid = 0; break; }
        Note *n = malloc(sizeof(Note));
        n->name = malloc(strlen(name) + 1);
        strcpy(n->name, name);
        n->note = malloc(strlen(note) + 1);
        strcpy(n->note, note);
        n->next = NULL;
        if (new_tail) {
            new_tail->next = n;
            new_tail = n;
        } else {
            new_head = new_tail = n;
        }
    }
    free(line);
    fclose(f);
    if (!valid) {
        free_store(new_head);
        return NULL;
    }
    return new_head;
}

void add_note(const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    n->name = malloc(strlen(name) + 1);
    strcpy(n->name, name);
    n->note = malloc(strlen(note) + 1);
    strcpy(n->note, note);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *prev = NULL, *curr = store;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else store = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes(void) {
    Note *curr = store;
    while (curr) {
        printf("%s %s\n", curr->name, curr->note);
        curr = curr->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    char *filename = argv[1];
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            Note *new_store = load_file(filename);
            if (new_store) {
                free_store(store);
                store = new_store;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free(line);
    free_store(store);
    return 0;
}