// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteRec;

NoteRec *notes = NULL;
int n_count = 0;

int find_name(const char *name) {
    for (int i = 0; i < n_count; i++) {
        if (strcmp(notes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void free_all_notes() {
    for (int i = 0; i < n_count; i++) {
        free(notes[i].name);
        free(notes[i].note);
    }
    free(notes);
    notes = NULL;
    n_count = 0;
}

int load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return 0;
    }
    NoteRec *tmp = NULL;
    int tmp_count = 0;
    char line[11264];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *sp = strchr(line, ' ');
        if (sp == NULL) {
            valid = 0;
            break;
        }
        *sp = '\0';
        tmp = realloc(tmp, sizeof(NoteRec) * (tmp_count + 1));
        tmp[tmp_count].name = strdup(line);
        tmp[tmp_count].note = strdup(sp + 1);
        tmp_count++;
    }
    fclose(f);
    if (!valid) {
        for (int i = 0; i < tmp_count; i++) {
            free(tmp[i].name);
            free(tmp[i].note);
        }
        free(tmp);
        return 0;
    }
    free_all_notes();
    notes = tmp;
    n_count = tmp_count;
    return 1;
}

int main(int argc, char **argv) {
    char cmd[32];
    char name[1024];
    char note[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                load_from_file(argv[1]);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            notes = realloc(notes, sizeof(NoteRec) * (n_count + 1));
            notes[n_count].name = strdup(name);
            notes[n_count].note = strdup(note);
            n_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(notes[idx].name);
                free(notes[idx].note);
                for (int i = idx; i < n_count - 1; i++) {
                    notes[i] = notes[i + 1];
                }
                n_count--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n_count; i++) {
                printf("%s %s\n", notes[i].name, notes[i].note);
            }
        }
    }

    free_all_notes();

    return 0;
}