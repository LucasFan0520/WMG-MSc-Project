// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
} Note;

static Note *notes = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void clear_notes(void) {
    for (size_t i = 0; i < count; i++) {
        free(notes[i].name);
        free(notes[i].text);
    }
    count = 0;
}

static void add_note(const char *name, const char *text) {
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Note *new = realloc(notes, capacity * sizeof(Note));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        notes = new;
    }
    notes[count].name = strdup(name);
    notes[count].text = strdup(text);
    if (!notes[count].name || !notes[count].text) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    // Read all records, store temporarily
    Note *temp = NULL;
    size_t temp_count = 0, temp_cap = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    int valid = 1;
    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        char name[256], text[4096];
        if (sscanf(line, "%255s %4095[^\n]", name, text) != 2) {
            valid = 0;
            break;
        }
        if (temp_count == temp_cap) {
            temp_cap = temp_cap ? temp_cap * 2 : 4;
            Note *new = realloc(temp, temp_cap * sizeof(Note));
            if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
            temp = new;
        }
        temp[temp_count].name = strdup(name);
        temp[temp_count].text = strdup(text);
        if (!temp[temp_count].name || !temp[temp_count].text) { fprintf(stderr, "Memory error\n"); exit(1); }
        temp_count++;
    }
    free(line);
    fclose(f);
    if (valid) {
        // Replace current notes with temp
        clear_notes();
        // Move temp to notes
        notes = temp;
        count = temp_count;
        capacity = temp_cap;
    } else {
        // Free temp
        for (size_t i = 0; i < temp_count; i++) {
            free(temp[i].name);
            free(temp[i].text);
        }
        free(temp);
    }
}

static void delete_note(const char *name) {
    size_t write = 0;
    for (size_t i = 0; i < count; i++) {
        if (strcmp(notes[i].name, name) == 0) {
            free(notes[i].name);
            free(notes[i].text);
        } else {
            if (write != i) notes[write] = notes[i];
            write++;
        }
    }
    count = write;
}

static void report_notes(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", notes[i].name, notes[i].text);
}

static void free_all(void) {
    clear_notes();
    free(notes);
    notes = NULL;
    capacity = 0;
}

int main(int argc, char **argv) {
    char *filepath = argc > 1 ? argv[1] : NULL;

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], text[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) load_file(filepath);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, text) == 2)
                add_note(name, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }

    free(line);
    free_all();
    return 0;
}