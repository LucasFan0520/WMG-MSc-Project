// filename: F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

static Note *notes = NULL;

static void cleanup(void) {
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free(tmp->name);
        free(tmp->text);
        free(tmp);
    }
}

static Note *find_note(const char *name) {
    for (Note *n = notes; n; n = n->next)
        if (strcmp(n->name, name) == 0)
            return n;
    return NULL;
}

static void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    if (!n->name || !n->text) {
        free(n->name);
        free(n->text);
        free(n);
        return;
    }
    n->next = notes;
    notes = n;
}

static void delete_note(const char *name) {
    Note *prev = NULL;
    for (Note *n = notes; n; n = n->next) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->name);
            free(n->text);
            free(n);
            return;
        }
        prev = n;
    }
}

static void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    // parse lines: first token name, rest text
    Note *new_list = NULL;
    Note *tail = NULL;
    char *line = NULL;
    size_t len = 0;
    int success = 1;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char *name = line;
        char *text = strchr(line, ' ');
        if (!text) { success = 0; break; }
        *text = '\0';
        ++text;
        Note *n = malloc(sizeof(Note));
        if (!n) { success = 0; break; }
        n->name = strdup(name);
        n->text = strdup(text);
        if (!n->name || !n->text) {
            free(n->name);
            free(n->text);
            free(n);
            success = 0;
            break;
        }
        n->next = NULL;
        if (tail) tail->next = n;
        else new_list = n;
        tail = n;
    }
    free(line);
    fclose(f);
    if (success) {
        // replace old notes
        cleanup();
        notes = new_list;
        // atexit will clean up, but we need to reset the global cleanup? We already freed old notes, but atexit will try again; we need to set notes to new list and let atexit handle. However cleanup() above freed old notes, but we also need to prevent double free. We'll just set notes = new_list and let atexit do final cleanup.
        // But we used cleanup() which also frees notes; we called it, but we must not call it again on new_list? Actually cleanup() is atexit, it will free notes. So we can't call cleanup() manually. Instead, we should free old notes without using cleanup() to avoid double free.
        // Simpler: we'll free old notes manually, then assign new_list.
        // We'll define a helper to free a list.
    } else {
        // free new_list
        while (new_list) {
            Note *tmp = new_list;
            new_list = new_list->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
    }
}

static void report_notes(void) {
    for (Note *n = notes; n; n = n->next)
        printf("%s %s\n", n->name, n->text);
}

int main(int argc, char **argv) {
    char *filename = NULL;
    if (argc > 1) filename = argv[1];
    // We do not load automatically; only on LOAD command.
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filename) {
                // free old notes
                while (notes) {
                    Note *tmp = notes;
                    notes = notes->next;
                    free(tmp->name);
                    free(tmp->text);
                    free(tmp);
                }
                load_file(filename);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free(line);
    return 0;
}