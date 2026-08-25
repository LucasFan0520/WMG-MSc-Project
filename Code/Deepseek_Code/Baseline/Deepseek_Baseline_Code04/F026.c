/* F026.c */
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
    Note *prev = NULL, *cur = notes;
    while (cur) {
        if (strcmp(cur->category, category) == 0) {
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            free(cur->category);
            free(cur->text);
            free(cur);
            cur = prev ? prev->next : notes;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *cur = notes;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            free(cur->category);
            free(cur->text);
            free(cur);
            cur = prev ? prev->next : notes;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = notes;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
        n = n->next;
    }
}

void print_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void free_notes(void) {
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char cat[256];
        int n = sscanf(line, "%s %s", cmd, cat);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(cat) + 1;
            while (*p == ' ') p++;
            if (*p) add_note(cat, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_category(cat);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (n < 2) continue;
            keep_category(cat);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_word(cat);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_notes();
    return 0;
}