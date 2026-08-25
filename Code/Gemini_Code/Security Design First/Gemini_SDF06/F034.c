// F034.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} FileNote;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    FileNote *store = NULL;
    size_t count = 0;
    size_t capacity = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                FileNote *temp_store = NULL;
                size_t temp_count = 0;
                size_t temp_capacity = 0;
                int valid = 1;

                char *f_line = NULL;
                size_t f_len = 0;
                while (getline(&f_line, &f_len, fp) != -1) {
                    f_line[strcspn(f_line, "\r\n")] = '\0';
                    char *fp_ptr = f_line;
                    while (*fp_ptr == ' ' || *fp_ptr == '\t') fp_ptr++;
                    char *name = fp_ptr;
                    while (*fp_ptr && *fp_ptr != ' ' && *fp_ptr != '\t') fp_ptr++;
                    if (*fp_ptr) {
                        *fp_ptr = '\0';
                        fp_ptr++;
                    }
                    while (*fp_ptr == ' ' || *fp_ptr == '\t') fp_ptr++;
                    char *note = fp_ptr;

                    if (*name == '\0') continue;

                    if (temp_count >= temp_capacity) {
                        size_t new_cap = temp_capacity == 0 ? 4 : temp_capacity * 2;
                        FileNote *new_t = realloc(temp_store, new_cap * sizeof(FileNote));
                        if (new_t) {
                            temp_store = new_t;
                            temp_capacity = new_cap;
                        } else {
                            valid = 0;
                            break;
                        }
                    }
                    char *nm = strdup(name);
                    char *nt = strdup(note);
                    if (nm && nt) {
                        temp_store[temp_count].name = nm;
                        temp_store[temp_count].note = nt;
                        temp_count++;
                    } else {
                        free(nm);
                        free(nt);
                        valid = 0;
                        break;
                    }
                }
                free(f_line);
                fclose(fp);

                if (valid) {
                    for (size_t i = 0; i < count; i++) {
                        free(store[i].name);
                        free(store[i].note);
                    }
                    free(store);
                    store = temp_store;
                    count = temp_count;
                    capacity = temp_capacity;
                } else {
                    for (size_t i = 0; i < temp_count; i++) {
                        free(temp_store[i].name);
                        free(temp_store[i].note);
                    }
                    free(temp_store);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*name == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                FileNote *new_store = realloc(store, new_cap * sizeof(FileNote));
                if (!new_store) continue;
                store = new_store;
                capacity = new_cap;
            }
            char *nm = strdup(name);
            char *nt = strdup(note);
            if (nm && nt) {
                store[count].name = nm;
                store[count].note = nt;
                count++;
            } else {
                free(nm);
                free(nt);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    free(line);
    return 0;
}
