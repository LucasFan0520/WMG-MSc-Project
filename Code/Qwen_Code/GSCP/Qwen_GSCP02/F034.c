/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

static Note *notes = NULL;
static int ncount = 0;
static int ncap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_note(const char *name) {
    for (int i = 0; i < ncount; i++)
        if (strcmp(notes[i].name, name) == 0) return i;
    return -1;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *temp = NULL;
    int tcount = 0;
    int tcap = 0;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        char name[256] = {0};
        int off = 0;
        if (sscanf(line, "%255s%n", name, &off) != 1) { valid = 0; break; }
        const char *note = line + off;
        while (*note == ' ') note++;
        if (tcount >= tcap) {
            tcap = tcap == 0 ? 8 : tcap * 2;
            temp = realloc(temp, sizeof(Note) * tcap);
            if (!temp) { valid = 0; break; }
        }
        temp[tcount].name = dup_str(name);
        temp[tcount].note = dup_str(note);
        if (!temp[tcount].name || !temp[tcount].note) { valid = 0; break; }
        tcount++;
    }
    fclose(f);
    if (valid) {
        for (int i = 0; i < ncount; i++) {
            free(notes[i].name);
            free(notes[i].note);
        }
        free(notes);
        notes = temp;
        ncount = tcount;
        ncap = tcap;
    } else {
        for (int i = 0; i < tcount; i++) {
            free(temp[i].name);
            free(temp[i].note);
        }
        free(temp);
    }
}

int main(int argc, char *argv[]) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", name, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            int idx = find_note(name);
            if (idx >= 0) {
                free(notes[idx].note);
                notes[idx].note = dup_str(note);
            } else {
                if (ncount >= ncap) {
                    ncap = ncap == 0 ? 8 : ncap * 2;
                    notes = realloc(notes, sizeof(Note) * ncap);
                }
                notes[ncount].name = dup_str(name);
                notes[ncount].note = dup_str(note);
                ncount++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int idx = find_note(name);
            if (idx < 0) continue;
            free(notes[idx].name);
            free(notes[idx].note);
            for (int i = idx; i < ncount - 1; i++) notes[i] = notes[i + 1];
            ncount--;
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ncount; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    for (int i = 0; i < ncount; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    return 0;
}