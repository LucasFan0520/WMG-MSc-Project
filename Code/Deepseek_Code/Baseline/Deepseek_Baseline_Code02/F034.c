// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;

Note *find_note(const char *name) {
    Note *n = head;
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
    n->next = head;
    head = n;
}

void delete_note(const char *name) {
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
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
    Note *n = head;
    while (n) {
        printf("%s:", n->name);
        for (char *c = n->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        n = n->next;
    }
}

void load_file(const char *fname, int *ok) {
    FILE *f = fopen(fname, "r");
    if (!f) { *ok = 0; return; }
    Note *new_head = NULL;
    Note *new_tail = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *p = line;
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (!*p) { valid = 0; break; }
        *p++ = 0;
        while (*p == ' ') p++;
        Note *n = malloc(sizeof(Note));
        n->name = malloc(strlen(name) + 1);
        strcpy(n->name, name);
        n->text = malloc(strlen(p) + 1);
        strcpy(n->text, p);
        n->next = NULL;
        if (new_tail) new_tail->next = n;
        else new_head = n;
        new_tail = n;
    }
    fclose(f);
    if (valid) {
        // replace current store
        // free old
        Note *old = head;
        while (old) {
            Note *next = old->next;
            free(old->name);
            free(old->text);
            free(old);
            old = next;
        }
        head = new_head;
        *ok = 1;
    } else {
        // free new list
        Note *n = new_head;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->text);
            free(n);
            n = next;
        }
        *ok = 0;
    }
}

void free_all(void) {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = next;
    }
}

int main(int argc, char **argv) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (argc > 1) {
                int ok;
                load_file(argv[1], &ok);
                // if not ok, previous store remains (we already handle)
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_note(name, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_note(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_all();
    return 0;
}