// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = safe_strdup(category);
    n->text = safe_strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note *prev = NULL, *cur = notes;
    while (cur) {
        if (strcmp(cur->category, category) == 0) {
            Note *to_free = cur;
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            cur = cur->next;
            free(to_free->category);
            free(to_free->text);
            free(to_free);
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *cur = notes;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            Note *to_free = cur;
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            cur = cur->next;
            free(to_free->category);
            free(to_free->text);
            free(to_free);
            continue;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_word(const char *word) {
    Note *cur = notes;
    int found = 0;
    while (cur) {
        if (strstr(cur->text, word)) {
            printf("%s %s\n", cur->category, cur->text);
            found = 1;
        }
        cur = cur->next;
    }
    if (!found) printf("NOT_FOUND\n");
}

void print_notes(void) {
    Note *cur = notes;
    while (cur) {
        printf("%s %s\n", cur->category, cur->text);
        cur = cur->next;
    }
}

void free_all(void) {
    Note *cur = notes;
    while (cur) {
        Note *next = cur->next;
        free(cur->category);
        free(cur->text);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_note(arg1, p);
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
    free_all();
    return 0;
}