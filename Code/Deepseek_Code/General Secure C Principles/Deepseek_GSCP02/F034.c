// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
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
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *prev = NULL;
    Note *n = store;
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

void report_notes(void) {
    Note *n = store;
    while (n) {
        printf("%s ", n->name);
        for (char *p = n->text; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        n = n->next;
    }
}

void free_store(Note *s) {
    while (s) {
        Note *next = s->next;
        free(s->name);
        free(s->text);
        free(s);
        s = next;
    }
}

int load_from_file(const char *path, Note **new_store) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    Note *head = NULL;
    char line[1024];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[1024];
        if (sscanf(line, "%255s %1023[^\n]", name, text) != 2) {
            valid = 0;
            break;
        }
        Note *n = malloc(sizeof(Note));
        if (!n) { valid = 0; break; }
        n->name = strdup(name);
        n->text = strdup(text);
        n->next = head;
        head = n;
    }
    fclose(f);
    if (valid) {
        *new_store = head;
        return 1;
    } else {
        free_store(head);
        return 0;
    }
}

int main(int argc, char *argv[]) {
    char *filepath = (argc >= 2) ? argv[1] : NULL;

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "LOAD") == 0) {
            if (!filepath) continue;
            Note *new_store = NULL;
            if (load_from_file(filepath, &new_store)) {
                free_store(store);
                store = new_store;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256], text[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, text) == 2) {
                add_note(name, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_note(name);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }

    free(line);
    free_store(store);
    return 0;
}