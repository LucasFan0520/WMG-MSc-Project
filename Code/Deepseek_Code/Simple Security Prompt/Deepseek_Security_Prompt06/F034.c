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
    n->name = malloc(strlen(name) + 1);
    strcpy(n->name, name);
    n->text = malloc(strlen(text) + 1);
    strcpy(n->text, text);
    n->next = store;
    store = n;
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

void report_all(void) {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void load_store(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    Note *new_store = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[4096];
        if (sscanf(line, "%255s", name) != 1) { valid = 0; break; }
        const char *t = line + strlen(name) + 1;
        while (*t == ' ') t++;
        strcpy(text, t);
        // check duplicate in new_store
        Note *n = new_store;
        while (n) {
            if (strcmp(n->name, name) == 0) { valid = 0; break; }
            n = n->next;
        }
        if (!valid) break;
        Note *new = malloc(sizeof(Note));
        new->name = malloc(strlen(name) + 1);
        strcpy(new->name, name);
        new->text = malloc(strlen(text) + 1);
        strcpy(new->text, text);
        new->next = new_store;
        new_store = new;
    }
    fclose(fp);
    if (valid) {
        // replace store
        Note *old = store;
        while (old) {
            Note *tmp = old->next;
            free(old->name);
            free(old->text);
            free(old);
            old = tmp;
        }
        store = new_store;
    } else {
        // free new_store
        Note *n = new_store;
        while (n) {
            Note *tmp = n->next;
            free(n->name);
            free(n->text);
            free(n);
            n = tmp;
        }
    }
}

void free_all(void) {
    Note *n = store;
    while (n) {
        Note *tmp = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = tmp;
    }
}

int main(int argc, char **argv) {
    // The file path is provided, but we only load on LOAD command.
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], text[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_store(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *t = rest + strlen(name) + 1;
                while (*t == ' ') t++;
                if (*t) add_note(name, t);
                else add_note(name, "");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}