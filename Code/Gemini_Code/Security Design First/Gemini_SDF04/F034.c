// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteRecord;

NoteRecord *store = NULL;
int count = 0;
int capacity = 0;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *note = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        NoteRecord *tmp = realloc(store, capacity * sizeof(NoteRecord));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        store = tmp;
                    }
                    store[count].name = strdup(arg1);
                    store[count].note = strdup(note);
                    if (store[count].name && store[count].note) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(store[i].name, arg1) == 0) {
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
            }
        } else {
            if (strcmp(cmd, "LOAD") == 0) {
                FILE *f = fopen(argv[1], "r");
                if (f) {
                    char *fline = NULL;
                    size_t flen = 0;
                    ssize_t fnread;
                    int all_valid = 1;
                    NoteRecord *temp_store = NULL;
                    int temp_count = 0;
                    int temp_cap = 0;
                    while ((fnread = getline(&fline, &flen, f)) != -1) {
                        size_t fl = strlen(fline);
                        if (fl > 0 && fline[fl - 1] == '\n') {
                            fline[fl - 1] = '\0';
                        }
                        char *s = strchr(fline, ' ');
                        if (!s || s == fline) {
                            all_valid = 0;
                            break;
                        }
                        *s = '\0';
                        if (temp_count >= temp_cap) {
                            temp_cap = temp_cap == 0 ? 4 : temp_cap * 2;
                            NoteRecord *tmp = realloc(temp_store, temp_cap * sizeof(NoteRecord));
                            if (!tmp) {
                                all_valid = 0;
                                break;
                            }
                            temp_store = tmp;
                        }
                        temp_store[temp_count].name = strdup(fline);
                        temp_store[temp_count].note = strdup(s + 1);
                        if (temp_store[temp_count].name && temp_store[temp_count].note) {
                            temp_count++;
                        } else {
                            all_valid = 0;
                            break;
                        }
                    }
                    free(fline);
                    fclose(f);
                    if (all_valid) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = temp_store;
                        count = temp_count;
                        capacity = temp_cap;
                    } else {
                        for (int i = 0; i < temp_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", store[i].name, store[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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