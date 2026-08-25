// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL, *tail = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = malloc(strlen(category) + 1);
    strcpy(n->category, category);
    n->text = malloc(strlen(text) + 1);
    strcpy(n->text, text);
    n->next = NULL;
    if (tail) {
        tail->next = n;
        tail = n;
    } else {
        head = tail = n;
    }
}

void delete_category(const char *category) {
    Note *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->category, category) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Note *next = curr->next;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = next;
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
            Note *next = curr->next;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(const char *word) {
    Note *curr = head;
    while (curr) {
        if (strstr(curr->text, word)) {
            printf("%s %s\n", curr->category, curr->text);
        }
        curr = curr->next;
    }
}

void print_notes(void) {
    Note *curr = head;
    while (curr) {
        printf("%s %s\n", curr->category, curr->text);
        curr = curr->next;
    }
}

void free_all(void) {
    Note *curr = head;
    while (curr) {
        Note *next = curr->next;
        free(curr->category);
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *category = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (category && text) add_note(category, text);
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
            print_notes();
        }
    }
    free(line);
    free_all();
    return 0;
}