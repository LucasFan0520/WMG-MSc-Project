/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
} Note;

static Note *notes = NULL;
static size_t ncount = 0, ncap = 0;

static void add_note(const char *category, const char *text) {
    if (ncount == ncap) {
        ncap = ncap ? ncap * 2 : 4;
        notes = realloc(notes, ncap * sizeof(Note));
        if (!notes) { perror("realloc"); exit(1); }
    }
    notes[ncount].category = strdup(category);
    notes[ncount].text = strdup(text);
    ncount++;
}

static void delete_category(const char *category) {
    for (size_t i = 0; i < ncount; ) {
        if (strcmp(notes[i].category, category) == 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (size_t j = i; j < ncount - 1; j++)
                notes[j] = notes[j + 1];
            ncount--;
        } else {
            i++;
        }
    }
}

static void keep_category(const char *category) {
    for (size_t i = 0; i < ncount; ) {
        if (strcmp(notes[i].category, category) != 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (size_t j = i; j < ncount - 1; j++)
                notes[j] = notes[j + 1];
            ncount--;
        } else {
            i++;
        }
    }
}

static void find_word(const char *word) {
    for (size_t i = 0; i < ncount; i++) {
        if (strstr(notes[i].text, word)) {
            printf("%s\n", notes[i].text);
        }
    }
}

static void print_notes(void) {
    for (size_t i = 0; i < ncount; i++) {
        printf("%s %s\n", notes[i].category, notes[i].text);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = strtok(NULL, " ");
            if (!cat) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_note(cat, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = strtok(NULL, " ");
            if (cat) delete_category(cat);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = strtok(NULL, " ");
            if (cat) keep_category(cat);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = strtok(NULL, " ");
            if (word) find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    for (size_t i = 0; i < ncount; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
    return 0;
}