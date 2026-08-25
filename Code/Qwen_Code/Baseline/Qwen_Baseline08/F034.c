// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOTES 1000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char note[MAX_LEN];
} Note;

Note notes[MAX_NOTES];
int ncount = 0;

int find_note(const char *name) {
    for (int i = 0; i < ncount; i++) if (strcmp(notes[i].name, name) == 0) return i;
    return -1;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note temp[MAX_NOTES];
    int tcount = 0;
    char line[MAX_LEN];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        char *space = strchr(line, ' ');
        if (!space) {
            valid = 0;
            break;
        }
        *space = '\0';
        if (tcount < MAX_NOTES) {
            strcpy(temp[tcount].name, line);
            strcpy(temp[tcount].note, space + 1);
            tcount++;
        }
    }
    fclose(f);
    if (valid) {
        ncount = tcount;
        memcpy(notes, temp, sizeof(Note) * tcount);
    }
}

int main(int argc, char *argv[]) {
    char cmd[20], name[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find_note(name);
            if (idx == -1 && ncount < MAX_NOTES) {
                strcpy(notes[ncount].name, name);
                strcpy(notes[ncount].note, note);
                ncount++;
            } else if (idx != -1) {
                strcpy(notes[idx].note, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_note(name);
            if (idx != -1) {
                for (int i = idx; i < ncount - 1; i++) notes[i] = notes[i + 1];
                ncount--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ncount; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }
    return 0;
}