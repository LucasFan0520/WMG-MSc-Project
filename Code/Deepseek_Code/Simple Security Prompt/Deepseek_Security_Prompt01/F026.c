// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *category; char *text; } Note;
Note *notes = NULL;
int nc = 0, ncap = 0;

void add_note(const char *category, const char *text) {
    if (nc == ncap) { ncap = ncap ? ncap * 2 : 4; notes = realloc(notes, sizeof(Note) * ncap); }
    notes[nc].category = strdup(category);
    notes[nc].text = strdup(text);
    nc++;
}

void delete_category(const char *category) {
    int i = 0;
    while (i < nc) {
        if (strcmp(notes[i].category, category) == 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (int j = i; j < nc - 1; j++) notes[j] = notes[j + 1];
            nc--;
        } else i++;
    }
}

void keep_category(const char *category) {
    int i = 0;
    while (i < nc) {
        if (strcmp(notes[i].category, category) != 0) {
            free(notes[i].category);
            free(notes[i].text);
            for (int j = i; j < nc - 1; j++) notes[j] = notes[j + 1];
            nc--;
        } else i++;
    }
}

void find_word(const char *word) {
    for (int i = 0; i < nc; i++) {
        if (strstr(notes[i].text, word) != NULL)
            printf("%s\n", notes[i].text);
    }
}

void print_notes(void) {
    for (int i = 0; i < nc; i++)
        printf("%s %s\n", notes[i].category, notes[i].text);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, rest) == 2)
                add_note(arg1, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", rest) == 1) find_word(rest);
        } else if (strcmp(cmd, "PRINT") == 0) print_notes();
    }
    for (int i = 0; i < nc; i++) { free(notes[i].category); free(notes[i].text); }
    free(notes);
    return 0;
}