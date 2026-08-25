// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct note {
    char *category;
    char *text;
    struct note *next;
};

struct note *notes = NULL;

void add_note(char *category, char *text) {
    struct note *n = malloc(sizeof(struct note));
    n->category = malloc(strlen(category) + 1);
    strcpy(n->category, category);
    n->text = malloc(strlen(text) + 1);
    strcpy(n->text, text);
    n->next = notes;
    notes = n;
}

void delete_category(char *category) {
    struct note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(char *category) {
    struct note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void find_word(char *word) {
    struct note *n = notes;
    while (n) {
        if (strstr(n->text, word) != NULL) {
            printf("%s %s\n", n->category, n->text);
        }
        n = n->next;
    }
}

void print_notes() {
    struct note *n = notes;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_note(arg1, rest);
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
    return 0;
}