// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000

typedef struct {
    char name[MAX_NAME];
    char note[MAX_NOTE];
} Note;

Note store[1000];
int store_count = 0;

Note temp_store[1000];
int temp_count = 0;

int find_note(const char *name) {
    for (int i = 0; i < store_count; i++) if (strcmp(store[i].name, name) == 0) return i;
    return -1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    temp_count = 0;
    char line[1200];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        char *space = strchr(line, ' ');
        if (!space) { valid = 0; break; }
        *space = '\0';
        strcpy(temp_store[temp_count].name, line);
        strcpy(temp_store[temp_count].note, space + 1);
        temp_count++;
    }
    fclose(f);
    if (valid) {
        memcpy(store, temp_store, sizeof(Note) * temp_count);
        store_count = temp_count;
    }
}

int main(int argc, char *argv[]) {
    char cmd[20], name[MAX_NAME], note[MAX_NOTE];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            strcpy(store[store_count].name, name);
            strcpy(store[store_count].note, note);
            store_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_note(name);
            if (idx != -1) {
                for (int i = idx; i < store_count - 1; i++) store[i] = store[i + 1];
                store_count--;
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < store_count; i++) printf("%s %s\n", store[i].name, store[i].note);
        }
    }
    return 0;
}