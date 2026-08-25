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
    Note *new = malloc(sizeof(Note));
    new->category = strdup(category);
    new->text = strdup(text);
    new->next = NULL;
    if (!head) head = new;
    else {
        Note *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void delete_category(const char *category) {
    Note *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->category, category) == 0) {
            Note *tmp = cur;
            if (prev) prev->next = cur->next;
            else head = cur->next;
            cur = cur->next;
            free(tmp->category);
            free(tmp->text);
            free(tmp);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            Note *tmp = cur;
            if (prev) prev->next = cur->next;
            else head = cur->next;
            cur = cur->next;
            free(tmp->category);
            free(tmp->text);
            free(tmp);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_word(const char *word) {
    Note *cur = head;
    while (cur) {
        if (strstr(cur->text, word) != NULL) {
            printf("%s %s\n", cur->category, cur->text);
        }
        cur = cur->next;
    }
}

void print_all() {
    Note *cur = head;
    while (cur) {
        printf("%s %s\n", cur->category, cur->text);
        cur = cur->next;
    }
}

void free_all() {
    Note *cur = head;
    while (cur) {
        Note *tmp = cur;
        cur = cur->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *category = strtok(NULL, " ");
            if (!category) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_note(category, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *category = strtok(NULL, " ");
            if (category) delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *category = strtok(NULL, " ");
            if (category) keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = strtok(NULL, " ");
            if (word) find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}