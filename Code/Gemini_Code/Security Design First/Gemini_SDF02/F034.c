/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteRec;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    NoteRec *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                NoteRec *new_store = NULL;
                size_t new_count = 0;
                size_t new_capacity = 0;
                char *f_line = NULL;
                size_t f_len = 0;
                int fully_valid = 1;

                while (getline(&f_line, &f_len, fp) != -1) {
                    size_t flen = strlen(f_line);
                    while (flen > 0 && (f_line[flen - 1] == '\n' || f_line[flen - 1] == '\r')) {
                        f_line[flen - 1] = '\0';
                        flen--;
                    }
                    char *fp_ptr = f_line;
                    while (*fp_ptr == ' ') fp_ptr++;
                    if (*fp_ptr == '\0') continue;
                    char *name = fp_ptr;
                    while (*fp_ptr && *fp_ptr != ' ') fp_ptr++;
                    if (*fp_ptr) { *fp_ptr = '\0'; fp_ptr++; }
                    while (*fp_ptr == ' ') fp_ptr++;
                    char *note = fp_ptr;

                    if (new_count >= new_capacity) {
                        size_t n_cap = new_capacity == 0 ? 4 : new_capacity * 2;
                        NoteRec *temp = realloc(new_store, n_cap * sizeof(NoteRec));
                        if (!temp) { fully_valid = 0; break; }
                        new_store = temp;
                        new_capacity = n_cap;
                    }
                    new_store[new_count].name = strdup(name);
                    new_store[new_count].note = strdup(note);
                    if (new_store[new_count].name && new_store[new_count].note) {
                        new_count++;
                    } else {
                        fully_valid = 0;
                        break;
                    }
                }
                free(f_line);
                fclose(fp);

                if (fully_valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = new_store;
                    count = new_count;
                    capacity = new_capacity;
                } else {
                    for (size_t i = 0; i < new_count; i++) {
                        free(new_store[i].name);
                        free(new_store[i].note);
                    }
                    free(new_store);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                NoteRec *temp = realloc(store, new_cap * sizeof(NoteRec));
                if (!temp) break;
                store = temp;
                capacity = new_cap;
            }
            store[count].name = strdup(name);
            store[count].note = strdup(note);
            if (store[count].name && store[count].note) count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
