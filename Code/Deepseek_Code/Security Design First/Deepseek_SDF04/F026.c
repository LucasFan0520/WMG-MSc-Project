// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = NULL;
    if (!n->category || !n->text) {
        free(n->category); free(n->text); free(n);
        return;
    }
    if (!head) head = n;
    else {
        Note *last = head;
        while (last->next) last = last->next;
        last->next = n;
    }
}

void delete_category(const char *category) {
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void keep_category(const char *category) {
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
        n = n->next;
    }
}

void print_notes(void) {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void free_all(void) {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], category[256], text[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", category, text) == 2)
                add_note(category, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", category) == 1)
                delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", category) == 1)
                keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", text) == 1) // text used as word
                find_word(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free(line);
    free_all();
    return 0;
}