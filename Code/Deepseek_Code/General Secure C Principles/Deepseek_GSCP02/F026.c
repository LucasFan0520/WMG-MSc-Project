// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *n_head = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = n_head;
    n_head = n;
}

void delete_category(const char *category) {
    Note *prev = NULL;
    Note *n = n_head;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else n_head = n->next;
            Note *tofree = n;
            n = n->next;
            free(tofree->category);
            free(tofree->text);
            free(tofree);
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(const char *category) {
    Note *prev = NULL;
    Note *n = n_head;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else n_head = n->next;
            Note *tofree = n;
            n = n->next;
            free(tofree->category);
            free(tofree->text);
            free(tofree);
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void find_word(const char *word) {
    Note *n = n_head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s ", n->category);
            for (char *p = n->text; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
        }
        n = n->next;
    }
}

void print_all(void) {
    Note *n = n_head;
    while (n) {
        printf("%s ", n->category);
        for (char *p = n->text; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        n = n->next;
    }
}

void free_all(void) {
    while (n_head) {
        Note *next = n_head->next;
        free(n_head->category);
        free(n_head->text);
        free(n_head);
        n_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char cat[256], text[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", cat, text) == 2) {
                add_note(cat, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            if (sscanf(line, "%*s %255s", cat) == 1) {
                delete_category(cat);
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            if (sscanf(line, "%*s %255s", cat) == 1) {
                keep_category(cat);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(line, "%*s %255s", word) == 1) {
                find_word(word);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }

    free(line);
    free_all();
    return 0;
}