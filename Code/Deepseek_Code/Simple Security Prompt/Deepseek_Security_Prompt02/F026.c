/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void free_note(Note *n) {
    free(n->category);
    free(n->text);
    free(n);
}

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note *prev = NULL, *curr = notes;
    while (curr) {
        if (strcmp(curr->category, category) == 0) {
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            free_note(curr);
            curr = prev ? prev->next : notes;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *curr = notes;
    while (curr) {
        if (strcmp(curr->category, category) != 0) {
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            free_note(curr);
            curr = prev ? prev->next : notes;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = notes;
    int found = 0;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
            found = 1;
        }
        n = n->next;
    }
    if (!found) printf("NOT_FOUND\n");
}

void print_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char category[256], text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", category) != 1) continue;
            p += strlen(category);
            while (*p == ' ') p++;
            add_note(category, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", category) == 1) delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", category) == 1) keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            find_word(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }

    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free_note(tmp);
    }
    return 0;
}