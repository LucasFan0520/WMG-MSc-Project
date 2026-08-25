// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
} FileNote;

int main(int argc, char *argv[]) {
    FileNote *store = NULL;
    int count = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    FileNote *temp_store = NULL;
                    int temp_count = 0;
                    int success = 1;
                    while (1) {
                        char *f_line = read_line(fp);
                        if (!f_line) break;
                        char *p = f_line;
                        while (*p && *p != ' ') p++;
                        if (*p == ' ') {
                            *p = '\0';
                            char *name = f_line;
                            char *note = p + 1;
                            temp_store = realloc(temp_store, (temp_count + 1) * sizeof(FileNote));
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        } else {
                            success = 0;
                            free(f_line);
                            break;
                        }
                        free(f_line);
                    }
                    fclose(fp);
                    if (success) {
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
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                store = realloc(store, (count + 1) * sizeof(FileNote));
                store[count].name = strdup(name);
                store[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    store = realloc(store, count * sizeof(FileNote));
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
