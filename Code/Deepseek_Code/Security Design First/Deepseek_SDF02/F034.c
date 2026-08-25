/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *name;
    char *text;
    struct note *next;
} note_t;

note_t *store = NULL;
note_t *tail = NULL;

note_t *find_note(const char *name) {
    note_t *n = store;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    note_t *n = malloc(sizeof(note_t));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = NULL;
    if (!n->name || !n->text) {
        free(n->name); free(n->text); free(n);
        return;
    }
    if (tail) tail->next = n;
    else store = n;
    tail = n;
}

void delete_note(const char *name) {
    note_t *prev = NULL, *n = store;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else store = n->next;
            if (n == tail) tail = prev;
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
    note_t *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void free_store(note_t *s) {
    note_t *n = s;
    while (n) {
        note_t *nxt = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = nxt;
    }
}

int load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    note_t *new_store = NULL;
    note_t *new_tail = NULL;
    char *line = NULL;
    size_t len = 0;
    int ok = 1;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = 0;
        char name[256];
        if (sscanf(line, "%255s", name) == 1) {
            const char *text = line + strlen(name) + 1;
            note_t *n = malloc(sizeof(note_t));
            if (!n) { ok = 0; break; }
            n->name = strdup(name);
            n->text = strdup(text);
            n->next = NULL;
            if (!n->name || !n->text) {
                free(n->name); free(n->text); free(n);
                ok = 0; break;
            }
            if (new_tail) new_tail->next = n;
            else new_store = n;
            new_tail = n;
        }
    }
    free(line);
    fclose(f);
    if (ok) {
        free_store(store);
        store = new_store;
        tail = new_tail;
    } else {
        free_store(new_store);
    }
    return ok;
}

void free_all(void) {
    free_store(store);
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "LOAD") == 0) {
                load_file(argv[1]);
            } else if (strcmp(cmd, "ADD") == 0) {
                const char *text = rest + strlen(a) + 1;
                add_note(a, text);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_note(a);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_notes();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}