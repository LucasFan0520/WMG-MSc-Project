/* F034.c */
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

void free_notes(Note *list) {
    while (list) {
        Note *tmp = list;
        list = list->next;
        free(tmp->name);
        free(tmp->text);
        free(tmp);
    }
}

void add_note(Note **list, const char *name, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = *list;
    *list = n;
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

void report_notes(void) {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = NULL;
    int valid = 1;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[2048];
        if (sscanf(line, "%255s %2047s", name, text) < 2) {
            valid = 0;
            break;
        }
        add_note(&temp, name, text);
    }
    fclose(f);
    if (valid) {
        free_notes(store);
        store = temp;
    } else {
        free_notes(temp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_from_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "LOAD") == 0 && n >= 2) {
            load_from_file(arg1);
        } else if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_note(&store, arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_notes(store);
    return 0;
}