/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note34;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    Note34 *store = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                char *fline = NULL;
                size_t flen = 0;
                ssize_t fread_bytes;
                Note34 *tmp_store = NULL;
                int tmp_count = 0;
                int tmp_cap = 0;
                int load_ok = 1;
                while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
                    fline[strcspn(fline, "\r\n")] = '\0';
                    char *space = strchr(fline, ' ');
                    if (!space) continue;
                    *space = '\0';
                    char *name = fline;
                    char *note = space + 1;
                    char *n_name = strdup(name);
                    char *n_note = strdup(note);
                    if (n_name && n_note) {
                        if (tmp_count >= tmp_cap) {
                            int new_cap = tmp_cap == 0 ? 4 : tmp_cap * 2;
                            Note34 *t = realloc(tmp_store, new_cap * sizeof(Note34));
                            if (t) {
                                tmp_store = t;
                                tmp_cap = new_cap;
                            } else {
                                free(n_name); free(n_note);
                                load_ok = 0;
                                break;
                            }
                        }
                        tmp_store[tmp_count].name = n_name;
                        tmp_store[tmp_count].note = n_note;
                        tmp_count++;
                    } else {
                        free(n_name); free(n_note);
                        load_ok = 0;
                        break;
                    }
                }
                free(fline);
                fclose(fp);
                if (load_ok) {
                    for (int i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = tmp_store;
                    count = tmp_count;
                    capacity = tmp_cap;
                } else {
                    for (int i = 0; i < tmp_count; i++) {
                        free(tmp_store[i].name);
                        free(tmp_store[i].note);
                    }
                    free(tmp_store);
                }
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *name = NULL;
            char *note = NULL;
            if (space) {
                *space = '\0';
                name = args;
                note = space + 1;
            } else {
                name = args;
                note = "";
            }
            char *n_name = strdup(name);
            char *n_note = strdup(note);
            if (n_name && n_note) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Note34 *tmp = realloc(store, new_cap * sizeof(Note34));
                    if (tmp) {
                        store = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_note);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                store[count].name = n_name;
                store[count].note = n_note;
                count++;
            } else {
                free(n_name); free(n_note);
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
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
