// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *head = NULL;

void clear_store() {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->note);
        free(n);
        n = next;
    }
    head = NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    
    Note *new_head = NULL;
    char line[8192];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[1024] = {0};
        char note[4096] = {0};
        if (sscanf(line, "%1023s %4095[^\n]", name, note) == 2) {
            Note *n = malloc(sizeof(Note));
            if (!n) { valid = 0; break; }
            n->name = strdup(name);
            n->note = strdup(note);
            if (!n->name || !n->note) {
                free(n->name); free(n->note); free(n);
                valid = 0; break;
            }
            n->next = new_head;
            new_head = n;
        }
    }
    fclose(f);
    
    if (valid) {
        clear_store();
        head = new_head;
    } else {
        Note *n = new_head;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->note);
            free(n);
            n = next;
        }
    }
}

void add_note(const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->note = strdup(note);
    if (!n->name || !n->note) { free(n->name); free(n->note); free(n); return; }
    n->next = head;
    head = n;
}

void delete_note(const char *name) {
    Note *curr = head;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report() {
    for (Note *n = head; n; n = n->next) {
        printf("%s %s\n", n->name, n->note);
    }
}

void free_all() {
    clear_store();
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "LOAD") == 0 && argc > 1) {
            load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free_all();
    return 0;
}