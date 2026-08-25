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
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            Note *next = n->next;
            if (prev) prev->next = next;
            else head = next;
            free(n->category);
            free(n->text);
            free(n);
            n = next;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(const char *category) {
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        Note *next = n->next;
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = next;
            else head = next;
            free(n->category);
            free(n->text);
            free(n);
        } else {
            prev = n;
        }
        n = next;
    }
}

void find_word(const char *word) {
    Note *n = head;
    int found = 0;
    while (n) {
        if (strstr(n->text, word) != NULL) {
            if (found) putchar(' ');
            printf("%s", n->text);
            found = 1;
        }
        n = n->next;
    }
    if (found) putchar('\n');
}

void print_notes(void) {
    Note *n = head;
    while (n) {
        printf("%s:", n->category);
        for (char *c = n->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *cat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_note(cat, p);
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