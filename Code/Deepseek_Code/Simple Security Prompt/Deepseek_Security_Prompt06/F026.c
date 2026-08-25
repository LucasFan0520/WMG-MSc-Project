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
    n->category = malloc(strlen(category) + 1);
    strcpy(n->category, category);
    n->text = malloc(strlen(text) + 1);
    strcpy(n->text, text);
    n->next = head;
    head = n;
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
            continue;
        }
        prev = n;
        n = n->next;
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
            continue;
        }
        prev = n;
        n = n->next;
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

void print_all(void) {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void free_all(void) {
    Note *n = head;
    while (n) {
        Note *tmp = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char category[256], text[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", category) == 1) {
                const char *t = rest + strlen(category) + 1;
                while (*t == ' ') t++;
                if (*t) add_note(category, t);
                else add_note(category, "");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", category) == 1) delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(rest, "%255s", category) == 1) keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%4095s", text) == 1) find_word(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}