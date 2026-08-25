// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line_file(FILE *f) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

typedef struct { char *name; char *note; } Note;
Note *notes = NULL;
size_t nc = 0, ncap = 0;

void add_note(const char *name, const char *note) {
    for (size_t i = 0; i < nc; i++) {
        if (strcmp(notes[i].name, name) == 0) {
            char *tmp = strdup(note);
            if (!tmp) { perror("strdup"); exit(1); }
            free(notes[i].note);
            notes[i].note = tmp;
            return;
        }
    }
    if (nc == ncap) {
        size_t nnc = ncap ? ncap * 2 : 4;
        if (nnc > SIZE_MAX / sizeof(Note)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Note *tmp = realloc(notes, nnc * sizeof(Note));
        if (!tmp) { perror("realloc"); exit(1); }
        notes = tmp;
        ncap = nnc;
    }
    notes[nc].name = strdup(name);
    notes[nc].note = strdup(note);
    if (!notes[nc].name || !notes[nc].note) { perror("strdup"); exit(1); }
    nc++;
}

void del_note(const char *name) {
    for (size_t i = 0; i < nc; i++) {
        if (strcmp(notes[i].name, name) == 0) {
            free(notes[i].name); free(notes[i].note);
            notes[i] = notes[--nc];
            return;
        }
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *tmp_notes = NULL;
    size_t tnc = 0, tncap = 0;
    char *line;
    int valid = 1;
    while ((line = read_line_file(f)) != NULL) {
        char name[256];
        if (sscanf(line, "%255s", name) != 1) { valid = 0; free(line); break; }
        char *note = strchr(line, ' ');
        if (note) note++; else note = "";
        if (tnc == tncap) {
            size_t nnc = tncap ? tncap * 2 : 4;
            if (nnc > SIZE_MAX / sizeof(Note)) { valid = 0; free(line); break; }
            Note *tmp = realloc(tmp_notes, nnc * sizeof(Note));
            if (!tmp) { valid = 0; free(line); break; }
            tmp_notes = tmp;
            tncap = nnc;
        }
        tmp_notes[tnc].name = strdup(name);
        tmp_notes[tnc].note = strdup(note);
        if (!tmp_notes[tnc].name || !tmp_notes[tnc].note) { valid = 0; free(line); break; }
        tnc++;
        free(line);
    }
    fclose(f);
    if (valid) {
        for (size_t i = 0; i < nc; i++) { free(notes[i].name); free(notes[i].note); }
        free(notes);
        notes = tmp_notes;
        nc = tnc;
        ncap = tncap;
    } else {
        for (size_t i = 0; i < tnc; i++) { free(tmp_notes[i].name); free(tmp_notes[i].note); }
        free(tmp_notes);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < nc; i++) printf("%s %s\n", notes[i].name, notes[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < nc; i++) { free(notes[i].name); free(notes[i].note); }
    free(notes);
    return 0;
}