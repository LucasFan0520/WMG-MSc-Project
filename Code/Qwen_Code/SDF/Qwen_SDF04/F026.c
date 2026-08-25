// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* category; char* text; } Note;

Note* notes = NULL;
size_t n_count = 0, n_cap = 0;

void cleanup() {
    for (size_t i = 0; i < n_count; i++) {
        free(notes[i].category);
        free(notes[i].text);
    }
    free(notes);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* cat = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;

            if (n_count == n_cap) {
                size_t new_cap = n_cap == 0 ? 4 : n_cap * 2;
                Note* temp = realloc(notes, new_cap * sizeof(Note));
                if (!temp) { cleanup(); free(line); return 1; }
                notes = temp; n_cap = new_cap;
            }
            notes[n_count].category = strdup(cat);
            notes[n_count].text = strdup(text);
            if (!notes[n_count].category || !notes[n_count].text) { cleanup(); free(line); return 1; }
            n_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < n_count; ) {
                if (strcmp(notes[i].category, ptr) == 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < n_count - 1; j++) notes[j] = notes[j + 1];
                    n_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < n_count; ) {
                if (strcmp(notes[i].category, ptr) != 0) {
                    free(notes[i].category);
                    free(notes[i].text);
                    for (size_t j = i; j < n_count - 1; j++) notes[j] = notes[j + 1];
                    n_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < n_count; i++) {
                if (strstr(notes[i].text, ptr) != NULL) {
                    printf("%s\n", notes[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < n_count; i++) {
                printf("%s %s\n", notes[i].category, notes[i].text);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}