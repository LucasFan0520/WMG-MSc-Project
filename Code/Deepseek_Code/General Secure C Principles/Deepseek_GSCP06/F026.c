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
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note *cur = notes, *prev = NULL;
    while (cur) {
        if (strcmp(cur->category, category) == 0) {
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            free(cur->category);
            free(cur->text);
            free(cur);
            if (prev) cur = prev->next;
            else cur = notes;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void keep_category(const char *category) {
    Note *cur = notes, *prev = NULL;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            free(cur->category);
            free(cur->text);
            free(cur);
            if (prev) cur = prev->next;
            else cur = notes;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_word(const char *word) {
    Note *cur = notes;
    while (cur) {
        if (strstr(cur->text, word)) {
            printf("%s %s\n", cur->category, cur->text);
        }
        cur = cur->next;
    }
}

void print_notes(void) {
    Note *cur = notes;
    while (cur) {
        printf("%s %s\n", cur->category, cur->text);
        cur = cur->next;
    }
}

void cleanup_notes(void) {
    Note *cur = notes;
    while (cur) {
        Note *tmp = cur;
        cur = cur->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, arg1, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *text = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_note(arg1, text);
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
    cleanup_notes();
    return 0;
}