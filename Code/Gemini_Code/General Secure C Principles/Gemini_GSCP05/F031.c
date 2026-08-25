// F031.c
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
} Record31;

int main(void) {
    Record31 *store = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            long n = strtol(line + 6, NULL, 10);
            int valid_batch = 1;
            char **b_lines = malloc(n * sizeof(char *));
            for (int i = 0; i < n; i++) {
                b_lines[i] = read_line(stdin);
                if (!b_lines[i]) {
                    valid_batch = 0;
                }
            }
            if (valid_batch) {
                for (int i = 0; i < n; i++) {
                    char *p = b_lines[i];
                    while (*p == ' ') p++;
                    char *name = p;
                    while (*p && *p != ' ') p++;
                    if (*p != ' ') {
                        valid_batch = 0;
                    }
                }
            }
            if (valid_batch) {
                for (int i = 0; i < n; i++) {
                    char *p = b_lines[i];
                    while (*p == ' ') p++;
                    char *name = p;
                    while (*p && *p != ' ') p++;
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Record31 *new_s = realloc(store, capacity * sizeof(Record31));
                        if (new_s) store = new_s;
                    }
                    if (count < capacity) {
                        store[count].name = strdup(name);
                        store[count].note = strdup(note);
                        count++;
                    }
                }
            }
            for (int i = 0; i < n; i++) {
                if (b_lines[i]) free(b_lines[i]);
            }
            free(b_lines);
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(store[i].name, name) == 0) {
                    printf("%s\n", store[i].note);
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
