// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Note;
Note *store = NULL;
int sc = 0, scap = 0;
Note *backup = NULL;
int bc = 0, bcap = 0;

int find_note(Note *arr, int count, const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(arr[i].name, name) == 0) return i;
    return -1;
}

void add_note_internal(Note **arr, int *count, int *cap, const char *name, const char *note) {
    int idx = find_note(*arr, *count, name);
    if (idx >= 0) {
        free((*arr)[idx].note);
        (*arr)[idx].note = strdup(note);
    } else {
        if (*count == *cap) { *cap = *cap ? *cap * 2 : 4; *arr = realloc(*arr, sizeof(Note) * *cap); }
        (*arr)[*count].name = strdup(name);
        (*arr)[*count].note = strdup(note);
        (*count)++;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = NULL;
    int tcount = 0, tcap = 0;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[4096];
        if (sscanf(line, "%255s %4095[^\n]", name, note) != 2) { valid = 0; break; }
        add_note_internal(&temp, &tcount, &tcap, name, note);
    }
    fclose(f);
    if (valid) {
        // Backup current store
        if (bc == 0) {
            // store backup only once? We'll free old backup and copy current
            for (int i = 0; i < bc; i++) { free(backup[i].name); free(backup[i].note); }
            free(backup);
            bc = 0; bcap = 0;
            backup = NULL;
        }
        // Backup current store
        for (int i = 0; i < sc; i++) {
            add_note_internal(&backup, &bc, &bcap, store[i].name, store[i].note);
        }
        // Replace store with temp
        for (int i = 0; i < sc; i++) { free(store[i].name); free(store[i].note); }
        free(store);
        store = temp;
        sc = tcount;
        scap = tcap;
    } else {
        // invalid: discard temp
        for (int i = 0; i < tcount; i++) { free(temp[i].name); free(temp[i].note); }
        free(temp);
    }
}

void add_note(const char *name, const char *note) {
    add_note_internal(&store, &sc, &scap, name, note);
}

void delete_note(const char *name) {
    int idx = find_note(store, sc, name);
    if (idx < 0) return;
    free(store[idx].name);
    free(store[idx].note);
    for (int i = idx; i < sc - 1; i++) store[i] = store[i + 1];
    sc--;
}

void report_notes(void) {
    for (int i = 0; i < sc; i++)
        printf("%s %s\n", store[i].name, store[i].note);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) report_notes();
    }
    for (int i = 0; i < sc; i++) { free(store[i].name); free(store[i].note); }
    free(store);
    for (int i = 0; i < bc; i++) { free(backup[i].name); free(backup[i].note); }
    free(backup);
    return 0;
}