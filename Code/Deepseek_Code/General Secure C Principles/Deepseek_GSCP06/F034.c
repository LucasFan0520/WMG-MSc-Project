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

void clear_store(void) {
    Note *cur = store;
    while (cur) {
        Note *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    store = NULL;
}

void add_note(const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *cur = store, *prev = NULL;
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

void report_notes(void) {
    Note *cur = store;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void cleanup_store(void) {
    clear_store();
}

int main(int argc, char **argv) {
    char *filepath = (argc > 1) ? argv[1] : NULL;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (!filepath) continue;
            FILE *f = fopen(filepath, "r");
            if (!f) continue;
            // backup current store
            Note *backup = store;
            store = NULL;
            int valid = 1;
            char buf[4096];
            while (fgets(buf, sizeof(buf), f)) {
                buf[strcspn(buf, "\n")] = 0;
                char nname[256], nnote[2048];
                if (sscanf(buf, "%255s %2047[^\n]", nname, nnote) != 2) {
                    valid = 0;
                    break;
                }
                add_note(nname, nnote);
            }
            fclose(f);
            if (!valid) {
                // rollback
                clear_store();
                store = backup;
            } else {
                // discard backup
                clear_store(); // clear backup? Actually we need to free backup if not used
                // backup is now old store, we want to discard it
                Note *cur = backup;
                while (cur) {
                    Note *tmp = cur;
                    cur = cur->next;
                    free(tmp->name);
                    free(tmp->note);
                    free(tmp);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    cleanup_store();
    return 0;
}