/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;
Note *tail = NULL;

void clear_store(void) {
    Note *n = head;
    while (n) {
        Note *tmp = n;
        n = n->next;
        free(tmp->name);
        free(tmp->text);
        free(tmp);
    }
    head = tail = NULL;
}

void add_note(const char *name, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_note(const char *name) {
    Note *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes(void) {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    // backup current store
    Note *backup_head = head;
    Note *backup_tail = tail;
    head = tail = NULL;
    int ok = 1;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256];
        char *p = line;
        while (*p == ' ') p++;
        if (sscanf(p, "%255s", name) == 1) {
            char *text = p;
            while (*text && *text != ' ') text++;
            while (*text && *text == ' ') text++;
            if (*text) {
                add_note(name, text);
            } else {
                // invalid, treat as error
                ok = 0;
                break;
            }
        } else {
            ok = 0;
            break;
        }
    }
    fclose(f);
    if (!ok) {
        // restore backup
        clear_store();
        head = backup_head;
        tail = backup_tail;
    } else {
        // discard backup
        Note *n = backup_head;
        while (n) {
            Note *tmp = n;
            n = n->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
    }
}

void free_all(void) {
    clear_store();
}

int main(int argc, char **argv) {
    char filepath[256] = "";
    if (argc >= 2) {
        strncpy(filepath, argv[1], sizeof(filepath) - 1);
        filepath[sizeof(filepath) - 1] = '\0';
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath[0]) {
                load_from_file(filepath);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *text = p;
                while (*text && *text != ' ') text++;
                while (*text && *text == ' ') text++;
                if (*text) {
                    add_note(name, text);
                }
            }
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