// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} NoteRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    NoteRec *store = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *file_path = argc > 1 ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (file_path) {
                FILE *fp = fopen(file_path, "r");
                if (fp) {
                    NoteRec *new_store = NULL;
                    size_t new_count = 0;
                    size_t new_capacity = 0;
                    int success = 1;
                    while (1) {
                        char *fline = read_line(fp);
                        if (!fline) break;
                        char *b = fline;
                        while (*b == ' ') b++;
                        if (*b != '\0') {
                            char *name = b;
                            char *sp = strchr(b, ' ');
                            if (sp) {
                                *sp = '\0';
                                char *note = sp + 1;
                                while (*note == ' ') note++;
                                if (new_count >= new_capacity) {
                                    new_capacity = new_capacity == 0 ? 4 : new_capacity * 2;
                                    NoteRec *ns = realloc(new_store, new_capacity * sizeof(NoteRec));
                                    if (!ns) {
                                        success = 0;
                                        free(fline);
                                        break;
                                    }
                                    new_store = ns;
                                }
                                new_store[new_count].name = strdup(name);
                                new_store[new_count].note = strdup(note);
                                new_count++;
                            }
                        }
                        free(fline);
                    }
                    fclose(fp);
                    if (success) {
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
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                NoteRec *ns = realloc(store, capacity * sizeof(NoteRec));
                if (!ns) break;
                store = ns;
            }
            store[count].name = strdup(name);
            store[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(store[i].name);
        free(store[i].note);
    }
    free(store);
    return 0;
}
