// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *cat; char *text; } Note;
Note *notes = NULL;
size_t nc = 0, ncap = 0;

void add_note(const char *cat, const char *text) {
    if (nc == ncap) {
        size_t nc_cap = ncap ? ncap * 2 : 4;
        if (nc_cap > SIZE_MAX / sizeof(Note)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Note *tmp = realloc(notes, nc_cap * sizeof(Note));
        if (!tmp) { perror("realloc"); exit(1); }
        notes = tmp;
        ncap = nc_cap;
    }
    notes[nc].cat = strdup(cat);
    notes[nc].text = strdup(text);
    if (!notes[nc].cat || !notes[nc].text) { perror("strdup"); exit(1); }
    nc++;
}

void del_cat(const char *cat) {
    for (size_t i = 0; i < nc; ) {
        if (strcmp(notes[i].cat, cat) == 0) {
            free(notes[i].cat); free(notes[i].text);
            notes[i] = notes[--nc];
        } else i++;
    }
}

void keep_cat(const char *cat) {
    for (size_t i = 0; i < nc; ) {
        if (strcmp(notes[i].cat, cat) != 0) {
            free(notes[i].cat); free(notes[i].text);
            notes[i] = notes[--nc];
        } else i++;
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256];
            if (sscanf(line, "%*s %255s", cat) != 1) { free(line); continue; }
            char *text = strchr(line, ' ');
            if (text) { text = strchr(text + 1, ' '); if (text) text++; }
            if (!text) text = "";
            add_note(cat, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char cat[256];
            if (sscanf(line, "%*s %255s", cat) == 1) del_cat(cat);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char cat[256];
            if (sscanf(line, "%*s %255s", cat) == 1) keep_cat(cat);
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(line, "%*s %255s", word) == 1) {
                for (size_t i = 0; i < nc; i++) {
                    if (strstr(notes[i].text, word)) {
                        printf("%s\n", notes[i].text);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < nc; i++) printf("%s %s\n", notes[i].cat, notes[i].text);
        }
        free(line);
    }
    for (size_t i = 0; i < nc; i++) { free(notes[i].cat); free(notes[i].text); }
    free(notes);
    return 0;
}