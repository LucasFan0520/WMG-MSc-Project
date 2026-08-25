// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note;

Note *store = NULL;
int count = 0;
int capacity = 0;

int find_note(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            if (argc > 1) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    Note *temp_store = NULL;
                    int t_count = 0;
                    int t_capacity = 0;
                    char fline[4096];
                    int valid = 1;
                    while (fgets(fline, sizeof(fline), f)) {
                        fline[strcspn(fline, "\r\n")] = 0;
                        if (strlen(fline) == 0) continue;
                        char *sp = strchr(fline, ' ');
                        if (sp && sp != fline) {
                            *sp = 0;
                            if (t_count >= t_capacity) {
                                t_capacity = t_capacity == 0 ? 4 : t_capacity * 2;
                                temp_store = realloc(temp_store, t_capacity * sizeof(Note));
                            }
                            temp_store[t_count].name = strdup(fline);
                            temp_store[t_count].note = strdup(sp + 1);
                            t_count++;
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
                        count = t_count;
                        capacity = t_capacity;
                    } else {
                        for (int i = 0; i < t_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                int idx = find_note(p1);
                if (idx != -1) {
                    free(store[idx].note);
                    store[idx].note = strdup(sp + 1);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        store = realloc(store, capacity * sizeof(Note));
                    }
                    store[count].name = strdup(p1);
                    store[count].note = strdup(sp + 1);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_note(name);
            if (idx != -1) {
                free(store[idx].name);
                free(store[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    store[i] = store[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
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
