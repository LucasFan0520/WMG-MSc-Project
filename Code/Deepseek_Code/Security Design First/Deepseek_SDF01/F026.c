// filename: F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

static Note *notes = NULL;

static void cleanup(void) {
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

static void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    if (!n->category || !n->text) {
        free(n->category);
        free(n->text);
        free(n);
        return;
    }
    n->next = notes;
    notes = n;
}

static void delete_category(const char *category) {
    Note *prev = NULL;
    for (Note *n = notes; n; n = n->next) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev : notes;
            if (!n) break;
        }
        prev = n;
    }
}

static void keep_category(const char *category) {
    Note *prev = NULL;
    Note *cur = notes;
    while (cur) {
        Note *next = cur->next;
        if (strcmp(cur->category, category) != 0) {
            if (prev) prev->next = next;
            else notes = next;
            free(cur->category);
            free(cur->text);
            free(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
}

static void find_word(const char *word) {
    for (Note *n = notes; n; n = n->next) {
        if (strstr(n->text, word))
            printf("%s %s\n", n->category, n->text);
    }
}

static void print_notes(void) {
    for (Note *n = notes; n; n = n->next)
        printf("%s %s\n", n->category, n->text);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (*arg1) keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free(line);
    return 0;
}