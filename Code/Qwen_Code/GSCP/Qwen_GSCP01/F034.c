// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *notes = NULL;

void add_note(const char *name, const char *note) {
    Note *n = malloc(sizeof(Note));
    if (!n) exit(EXIT_FAILURE);
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note *curr = notes;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes() {
    Note *curr = notes;
    while (curr) {
        printf("%s %s\n", curr->name, curr->note);
        curr = curr->next;
    }
}

void free_notes() {
    Note *curr = notes;
    while (curr) {
        Note *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
    notes = NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    Note *temp_notes = NULL;
    char line[4096];
    int valid = 1;
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256] = {0};
        char note[3800] = {0};
        int matched = sscanf(line, "%255s %[^\n]", name, note);
        if (matched != 2) {
            valid = 0;
            break;
        }
        Note *n = malloc(sizeof(Note));
        if (!n) exit(EXIT_FAILURE);
        n->name = strdup(name);
        n->note = strdup(note);
        n->next = temp_notes;
        temp_notes = n;
    }
    fclose(f);
    
    if (valid) {
        free_notes();
        notes = temp_notes;
    } else {
        Note *curr = temp_notes;
        while (curr) {
            Note *next = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            curr = next;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 0;
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "LOAD") == 0) {
            load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0 && matched >= 2) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_notes();
    return 0;
}