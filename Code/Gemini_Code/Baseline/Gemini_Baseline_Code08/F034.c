// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

Note *store = NULL;
int count = 0;
int capacity = 0;

void add_note(const char *name, const char *note) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        store = realloc(store, capacity * sizeof(Note));
    }
    store[count].name = strdup(name);
    store[count].note = strdup(note);
    count++;
}

void delete_note(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            free(store[i].name);
            free(store[i].note);
            for (int j = i; j < count - 1; j++) {
                store[j] = store[j + 1];
            }
            count--;
            i--;
        }
    }
}

void report_notes(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", store[i].name, store[i].note);
    }
}

void load_from_file(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return;
    Note *new_store = NULL;
    int new_count = 0;
    int new_capacity = 0;
    char *f_line = NULL;
    size_t f_len = 0;
    ssize_t f_read;
    int valid = 1;
    while ((f_read = getline(&f_line, &f_len, fp)) != -1) {
        if (f_read > 0 && f_line[f_read - 1] == '\n') {
            f_line[f_read - 1] = '\0';
        }
        char *space = strchr(f_line, ' ');
        if (!space || space == f_line) {
            valid = 0;
            break;
        }
        if (new_count >= new_capacity) {
            new_capacity = new_capacity == 0 ? 4 : new_capacity * 2;
            new_store = realloc(new_store, new_capacity * sizeof(Note));
        }
        *space = '\0';
        new_store[new_count].name = strdup(f_line);
        new_store[new_count].note = strdup(space + 1);
        new_count++;
    }
    free(f_line);
    fclose(fp);
    if (valid) {
        for (int i = 0; i < count; i++) {
            free(store[i].name);
            free(store[i].note);
        }
        free(store);
        store = new_store;
        count = new_count;
        capacity = new_capacity;
    } else {
        for (int i = 0; i < new_count; i++) {
            free(new_store[i].name);
            free(new_store[i].note);
        }
        free(new_store);
    }
}

int main(int argc, char *argv[]) {
    char *filepath = (argc > 1) ? argv[1] : NULL;
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
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) load_from_file(filepath);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
