/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *category;
    char *text;
    struct note *next;
} note_t;

note_t *head = NULL;
note_t *tail = NULL;

void add_note(const char *category, const char *text) {
    note_t *n = malloc(sizeof(note_t));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = NULL;
    if (!n->category || !n->text) {
        free(n->category); free(n->text); free(n);
        return;
    }
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_category(const char *category) {
    note_t *prev = NULL, *n = head;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(const char *category) {
    note_t *prev = NULL, *n = head;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void find_word(const char *word) {
    note_t *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
        }
        n = n->next;
    }
}

void print_notes(void) {
    note_t *n = head;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void free_all(void) {
    note_t *n = head;
    while (n) {
        note_t *nxt = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256];
        if (sscanf(line, "%9s %255s", cmd, a) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                const char *text = rest + strlen(a) + 1;
                add_note(a, text);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_category(a);
            } else if (strcmp(cmd, "KEEP") == 0) {
                keep_category(a);
            } else if (strcmp(cmd, "FIND") == 0) {
                find_word(rest);
            } else if (strcmp(cmd, "PRINT") == 0) {
                print_notes();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}