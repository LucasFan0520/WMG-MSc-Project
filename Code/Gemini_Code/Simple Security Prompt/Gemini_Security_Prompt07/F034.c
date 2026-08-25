// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteItem;

int main(int argc, char **argv) {
    NoteItem *store = NULL;
    int count = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *f = fopen(filepath, "r");
                if (f) {
                    NoteItem *temp_store = NULL;
                    int temp_count = 0;
                    int valid = 1;
                    char fline[4096];
                    while (fgets(fline, sizeof(fline), f)) {
                        fline[strcspn(fline, "\r\n")] = 0;
                        char *fname = strtok(fline, " ");
                        char *fnote = strtok(NULL, "");
                        if (fname && fnote) {
                            temp_store = realloc(temp_store, (temp_count + 1) * sizeof(NoteItem));
                            temp_store[temp_count].name = strdup(fname);
                            temp_store[temp_count].note = strdup(fnote);
                            temp_count++;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    fclose(f);
                    if (valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = temp_store;
                        count = temp_count;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) {
                store = realloc(store, (count + 1) * sizeof(NoteItem));
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(store[i].name, name) == 0) {
                        free(store[i].name);
                        free(store[i].note);
                        for (int j = i; j < count - 1; j++) {
                            store[j] = store[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(store);
                    store = NULL;
                } else {
                    store = realloc(store, count * sizeof(NoteItem));
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", store[i].name, store[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
