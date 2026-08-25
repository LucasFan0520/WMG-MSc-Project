// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = safe_strdup(name);
    n->text = safe_strdup(text);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *prev = NULL, *cur = store;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else store = cur->next;
            free(cur->name);
            free(cur->text);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report_notes(void) {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void free_all(void) {
    Note *n = store;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    // Try to load all records, if any error, discard and keep old store.
    // We'll read into a temporary list.
    Note *new_store = NULL;
    Note *tail = NULL;
    char line[MAX_LINE];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[512], text[MAX_LINE];
        const char *p = line;
        while (*p && !isspace(*p)) p++;
        if (*p == '\0') { valid = 0; break; }
        int len_name = p - line;
        if (len_name >= 512) { valid = 0; break; }
        strncpy(name, line, len_name);
        name[len_name] = '\0';
        while (*p && isspace(*p)) p++;
        strncpy(text, p, sizeof(text)-1);
        text[sizeof(text)-1] = '\0';
        Note *n = malloc(sizeof(Note));
        if (!n) { valid = 0; break; }
        n->name = safe_strdup(name);
        n->text = safe_strdup(text);
        n->next = NULL;
        if (tail) tail->next = n;
        else new_store = n;
        tail = n;
    }
    fclose(f);
    if (valid) {
        // replace store
        free_all();
        store = new_store;
    } else {
        // free temporary
        Note *cur = new_store;
        while (cur) {
            Note *next = cur->next;
            free(cur->name);
            free(cur->text);
            free(cur);
            cur = next;
        }
    }
}

int main(int argc, char **argv) {
    char *filename = (argc >= 2) ? argv[1] : NULL;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "LOAD") == 0 && filename) {
            load_file(filename);
        } else if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_note(arg1, p);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_all();
    return 0;
}