/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
} Note;

static Note *notes = NULL;
static size_t ncount = 0, ncap = 0;

static void add_note(const char *name, const char *note) {
    if (ncount == ncap) {
        ncap = ncap ? ncap * 2 : 4;
        notes = realloc(notes, ncap * sizeof(Note));
        if (!notes) { perror("realloc"); exit(1); }
    }
    notes[ncount].name = strdup(name);
    notes[ncount].note = strdup(note);
    ncount++;
}

static void delete_note(const char *name) {
    for (size_t i = 0; i < ncount; i++) {
        if (strcmp(notes[i].name, name) == 0) {
            free(notes[i].name);
            free(notes[i].note);
            for (size_t j = i; j < ncount - 1; j++)
                notes[j] = notes[j + 1];
            ncount--;
            return;
        }
    }
}

static void report_notes(void) {
    for (size_t i = 0; i < ncount; i++) {
        printf("%s %s\n", notes[i].name, notes[i].note);
    }
}

static void clear_notes(void) {
    for (size_t i = 0; i < ncount; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    ncount = 0;
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    Note *temp = NULL;
    size_t tcount = 0, tcap = 0;
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *name = strtok(line, " ");
        if (!name || strlen(name) == 0) { valid = 0; break; }
        char *note = strtok(NULL, "");
        if (!note) note = "";
        if (tcount == tcap) {
            tcap = tcap ? tcap * 2 : 4;
            temp = realloc(temp, tcap * sizeof(Note));
            if (!temp) { perror("realloc"); exit(1); }
        }
        temp[tcount].name = strdup(name);
        temp[tcount].note = strdup(note);
        tcount++;
    }
    fclose(f);
    if (valid) {
        clear_notes();
        for (size_t i = 0; i < tcount; i++) {
            add_note(temp[i].name, temp[i].note);
        }
    }
    for (size_t i = 0; i < tcount; i++) {
        free(temp[i].name);
        free(temp[i].note);
    }
    free(temp);
}

int main(int argc, char **argv) {
    char *filepath = NULL;
    if (argc > 1) filepath = argv[1];
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) load_file(filepath);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    for (size_t i = 0; i < ncount; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    return 0;
}