// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) exit(EXIT_FAILURE);
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note *curr = notes;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->category, category) == 0) {
            Note *to_delete = curr;
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            curr = curr->next;
            free(to_delete->category);
            free(to_delete->text);
            free(to_delete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void keep_category(const char *category) {
    Note *curr = notes;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->category, category) != 0) {
            Note *to_delete = curr;
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            curr = curr->next;
            free(to_delete->category);
            free(to_delete->text);
            free(to_delete);
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(const char *word) {
    Note *curr = notes;
    while (curr) {
        if (strstr(curr->text, word)) {
            printf("%s\n", curr->text);
        }
        curr = curr->next;
    }
}

void print_notes() {
    Note *curr = notes;
    while (curr) {
        printf("%s\n", curr->text);
        curr = curr->next;
    }
}

void free_all() {
    Note *curr = notes;
    while (curr) {
        Note *next = curr->next;
        free(curr->category);
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 3) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_all();
    return 0;
}