// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteItem;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    NoteItem *store = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                NoteItem *new_store = NULL;
                int new_count = 0;
                char *fline = NULL;
                size_t fcap = 0;
                int ok = 1;
                while (getline(&fline, &fcap, fp) != -1) {
                    fline[strcspn(fline, "\r\n")] = 0;
                    char fname[256];
                    if (sscanf(fline, "%255s", fname) == 1) {
                        char *fnote = strchr(fline, ' ');
                        if (fnote) {
                            while (*fnote == ' ') fnote++;
                        }
                        if (!fnote) fnote = "";
                        NoteItem *tmp = realloc(new_store, (new_count + 1) * sizeof(NoteItem));
                        if (tmp) {
                            new_store = tmp;
                            new_store[new_count].name = strdup(fname);
                            new_store[new_count].note = strdup(fnote);
                            new_count++;
                        } else {
                            ok = 0;
                            break;
                        }
                    }
                }
                free(fline);
                fclose(fp);
                if (ok) {
                    for (int i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = new_store;
                    count = new_count;
                } else {
                    for (int i = 0; i < new_count; i++) {
                        free(new_store[i].name);
                        free(new_store[i].note);
                    }
                    free(new_store);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char name[256];
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *note = strchr(p, ' ');
                if (note) {
                    while (*note == ' ') note++;
                }
                if (!note) note = "";
                NoteItem *tmp = realloc(store, (count + 1) * sizeof(NoteItem));
                if (tmp) {
                    store = tmp;
                    store[count].name = strdup(name);
                    store[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int i = 0;
            while (i < count) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
