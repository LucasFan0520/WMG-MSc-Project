/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;
Note *tail = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_category(const char *category) {
    Note *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->category, category) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = prev ? prev->next : head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->category, category) != 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = prev ? prev->next : head;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
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
        Note *tmp = n;
        n = n->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char category[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", category) == 1) {
                char *text = p;
                while (*text && *text != ' ') text++;
                while (*text && *text == ' ') text++;
                if (*text) {
                    add_note(category, text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_category(p);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            keep_category(p);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_word(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_all();
    return 0;
}