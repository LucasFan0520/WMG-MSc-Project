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

void add_note(const char *cat, const char *txt) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(cat);
    n->text = strdup(txt);
    n->next = notes;
    notes = n;
}

void delete_category(const char *cat) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void keep_category(const char *cat) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) != 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = notes;
    while (n) {
        if (strstr(n->text, word) != NULL) {
            printf("%s %s\n", n->category, n->text);
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char *txt = line + strlen(cmd) + strlen(arg1) + 2;
            while (*txt == ' ') txt++;
            add_note(arg1, txt);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0 && n >= 2) {
            keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
    return 0;
}