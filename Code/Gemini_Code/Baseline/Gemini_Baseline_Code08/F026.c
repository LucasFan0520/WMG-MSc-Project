// F026.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

Note *notes = NULL;
int count = 0;
int capacity = 0;

void add_note(const char *cat, const char *text) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        notes = realloc(notes, capacity * sizeof(Note));
    }
    notes[count].category = strdup(cat);
    notes[count].text = strdup(text);
    count++;
}

void delete_category(const char *cat) {
    int i = 0;
    while (i < count) {
        if (strcmp(notes[i].category, cat) == 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (int j = i; j < count - 1; j++) {
                notes[j] = notes[j + 1];
            }
            count--;
        } else {
            i++;
        }
    }
}

void keep_category(const char *cat) {
    int i = 0;
    while (i < count) {
        if (strcmp(notes[i].category, cat) != 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (int j = i; j < count - 1; j++) {
                notes[j] = notes[j + 1];
            }
            count--;
        } else {
            i++;
        }
    }
}

void find_word(const char *word) {
    for (int i = 0; i < count; i++) {
        if (strstr(notes[i].text, word) != NULL) {
            printf("%s %s\n", notes[i].category, notes[i].text);
        }
    }
}

void print_notes(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", notes[i].category, notes[i].text);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (cat && text) add_note(cat, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = strtok(NULL, "");
            if (cat) delete_category(cat);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = strtok(NULL, "");
            if (cat) keep_category(cat);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = strtok(NULL, "");
            if (word) find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}
