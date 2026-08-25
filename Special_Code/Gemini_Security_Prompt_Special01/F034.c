// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} NoteStore;

int main(int argc, char **argv) {
    NoteStore *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *filepath = argc > 1 ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    NoteStore *temp_store = NULL;
                    size_t temp_count = 0;
                    size_t temp_capacity = 0;
                    int valid = 1;
                    while (1) {
                        char *fline = read_line(fp);
                        if (!fline) break;
                        char *name = fline;
                        while (*name == ' ') name++;
                        if (*name == '\0') {
                            free(fline);
                            continue;
                        }
                        char *name_end = name;
                        while (*name_end && *name_end != ' ') name_end++;
                        char *note = name_end;
                        if (*name_end != '\0') {
                            *name_end = '\0';
                            note++;
                        }
                        while (*note == ' ') note++;
                        if (temp_count >= temp_capacity) {
                            temp_capacity = temp_capacity == 0 ? 4 : temp_capacity * 2;
                            NoteStore *new_t = realloc(temp_store, temp_capacity * sizeof(NoteStore));
                            if (new_t) temp_store = new_t;
                        }
                        temp_store[temp_count].name = mystrdup(name);
                        temp_store[temp_count].note = mystrdup(note);
                        temp_count++;
                        free(fline);
                    }
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
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *note = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                NoteStore *new_s = realloc(store, capacity * sizeof(NoteStore));
                if (new_s) store = new_s;
            }
            store[count].name = mystrdup(name);
            store[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
