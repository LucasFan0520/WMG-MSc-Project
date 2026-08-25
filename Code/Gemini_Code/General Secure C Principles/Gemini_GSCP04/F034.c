// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *note;
} NoteStoreEntry;

int main(int argc, char **argv) {
    NoteStoreEntry *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *filepath = argc > 1 ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *f = fopen(filepath, "r");
                if (f) {
                    NoteStoreEntry *temp_store = NULL;
                    size_t temp_count = 0;
                    size_t temp_capacity = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line(f);
                        if (!fline) break;
                        char *fp = fline;
                        while (*fp == ' ' || *fp == '\t') fp++;
                        if (*fp == '\0') {
                            free(fline);
                            continue;
                        }
                        char *name = fp;
                        while (*fp && *fp != ' ' && *fp != '\t') fp++;
                        if (*fp) {
                            *fp = '\0';
                            fp++;
                        }
                        while (*fp == ' ' || *fp == '\t') fp++;
                        char *note = fp;
                        if (*name == '\0') {
                            valid = 0;
                            free(fline);
                            break;
                        }
                        if (temp_count >= temp_capacity) {
                            temp_capacity = temp_capacity == 0 ? 4 : temp_capacity * 2;
                            NoteStoreEntry *new_temp = realloc(temp_store, temp_capacity * sizeof(NoteStoreEntry));
                            if (new_temp) temp_store = new_temp;
                        }
                        if (temp_count < temp_capacity) {
                            temp_store[temp_count].name = strdup(name);
                            temp_store[temp_count].note = strdup(note);
                            temp_count++;
                        }
                        free(fline);
                    }
                    fclose(f);
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
            if (*name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    NoteStoreEntry *new_store = realloc(store, capacity * sizeof(NoteStoreEntry));
                    if (new_store) store = new_store;
                }
                if (count < capacity) {
                    store[count].name = strdup(name);
                    store[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
