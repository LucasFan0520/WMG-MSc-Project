// F026.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
} Note;

static Note *notes = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void add_note(const char *category, const char *text) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Note *new = realloc(notes, capacity * sizeof(Note));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        notes = new;
    }
    notes[count].category = strdup(category);
    notes[count].text = strdup(text);
    if (!notes[count].category || !notes[count].text) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_category(const char *category) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(notes[i].category, category) == 0) {
            free(notes[i].category);
            free(notes[i].text);
        } else {
            if (write != i) notes[write] = notes[i];
            write++;
        }
    }
    count = write;
}

static void keep_category(const char *category) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(notes[i].category, category) == 0) {
            if (write != i) notes[write] = notes[i];
            write++;
        } else {
            free(notes[i].category);
            free(notes[i].text);
        }
    }
    count = write;
}

static void find_word(const char *word) {
    for (size_t i = 0; i < count; i++) {
        if (strstr(notes[i].text, word))
            printf("%s %s\n", notes[i].category, notes[i].text);
    }
}

static void print_notes(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", notes[i].category, notes[i].text);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], category[256], text[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", category, text) == 2)
                add_note(category, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", category) == 1)
                delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", category) == 1)
                keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) == 1)
                find_word(text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }

    free(line);
    free_all();
    return 0;
}