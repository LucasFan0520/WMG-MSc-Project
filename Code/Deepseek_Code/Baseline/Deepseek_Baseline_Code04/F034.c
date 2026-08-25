/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *store = NULL;

void add_note(Note **list, const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = *list;
    *list = n;
}

void free_notes(Note *list) {
    while (list) {
        Note *tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

Note *copy_notes(Note *src) {
    Note *new_list = NULL;
    Note *cur = src;
    while (cur) {
        add_note(&new_list, cur->name, cur->note);
        cur = cur->next;
    }
    return new_list;
}

int load_file(const char *path, Note **target) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    Note *temp = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256];
        char *p = line;
        while (*p == ' ') p++;
        char *name_start = p;
        while (*p && *p != ' ') p++;
        if (*p == '\0') { valid = 0; break; }
        *p++ = '\0';
        while (*p == ' ') p++;
        if (*p == '\0') { valid = 0; break; }
        add_note(&temp, name_start, p);
    }
    fclose(f);
    if (valid) {
        *target = temp;
        return 1;
    } else {
        free_notes(temp);
        return 0;
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

void report_notes(void) {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->note);
        n = n->next;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int n = sscanf(line, "%s %s", cmd, name);
        if (strcmp(cmd, "LOAD") == 0) {
            Note *new_store = NULL;
            if (load_file(argv[1], &new_store)) {
                free_notes(store);
                store = new_store;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            if (*p) add_note(&store, name, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_notes(store);
    return 0;
}