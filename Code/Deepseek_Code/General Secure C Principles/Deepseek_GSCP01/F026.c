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
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) == 0) {
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

void keep_category(const char *category) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) != 0) {
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
        if (strstr(n->text, word)) {
            printf("%s ", n->category);
            for (char *c = n->text; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
        }
        n = n->next;
    }
}

void print_all(void) {
    Note *n = notes;
    while (n) {
        printf("%s ", n->category);
        for (char *c = n->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        n = n->next;
    }
}

void free_all(void) {
    Note *n = notes;
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
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char category[256];
            char *p = line + 4;
            if (sscanf(p, "%255s", category) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_note(category, p);
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
            print_all();
        }
    }
    free_all();
    free(line);
    return 0;
}