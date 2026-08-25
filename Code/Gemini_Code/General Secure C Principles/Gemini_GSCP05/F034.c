// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Note *store = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(line, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                Note *temp_store = NULL;
                int temp_count = 0;
                int temp_cap = 0;
                int ok = 1;
                while (1) {
                    char *fline = read_line(fp);
                    if (!fline) break;
                    char *p = fline;
                    while (*p == ' ') p++;
                    char *name = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        char *note = p + 1;
                        while (*note == ' ') note++;
                        if (temp_count >= temp_cap) {
                            temp_cap = temp_cap == 0 ? 4 : temp_cap * 2;
                            Note *new_t = realloc(temp_store, temp_cap * sizeof(Note));
                            if (new_t) temp_store = new_t;
                        }
                        if (temp_count < temp_cap) {
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        }
                    } else {
                        if (strlen(fline) > 0) ok = 0;
                    }
                    free(fline);
                }
                fclose(fp);
                if (ok) {
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
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Note *new_s = realloc(store, capacity * sizeof(Note));
                    if (new_s) store = new_s;
                }
                if (count < capacity) {
                    store[count].name = strdup(name);
                    store[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    free(store[i].name);
                    free(store[i].note);
                    for (int j = i; j < count - 1; j++) {
                        store[j] = store[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s: %s\n", store[i].name, store[i].note);
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
