// F026.c
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
    if (!n) return;
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
            Note *to_remove = cur;
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            cur = cur->next;
            free(to_remove->category);
            free(to_remove->text);
            free(to_remove);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = notes;
    while (n) {
        if (strstr(n->text, word) != NULL) {
            printf("%s: ", n->category);
            char *s = n->text;
            for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
            putchar('\n');
        }
        n = n->next;
    }
}

void print_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s: ", n->category);
        char *s = n->text;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *category = p;
            char *space = strchr(category, ' ');
            if (!space) continue;
            *space = '\0';
            char *text = space + 1;
            while (*text == ' ') text++;
            add_note(category, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *category = p;
            char *space = strchr(category, ' ');
            if (space) *space = '\0';
            delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *category = p;
            char *space = strchr(category, ' ');
            if (space) *space = '\0';
            keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *word = p;
            char *space = strchr(word, ' ');
            if (space) *space = '\0';
            find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_all();
    return 0;
}