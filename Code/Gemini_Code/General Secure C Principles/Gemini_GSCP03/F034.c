// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Note;

int main(int argc, char **argv) {
    char *filepath = (argc >= 2) ? argv[1] : NULL;
    Note *store = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", store[i].name, store[i].note);
            }
            free(line);
            continue;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    Note *temp_store = NULL;
                    int f_count = 0;
                    int f_cap = 0;
                    int success = 1;
                    while (1) {
                        char *fline = read_line(fp);
                        if (!fline) break;
                        char *fptr = fline;
                        while (*fptr == ' ') fptr++;
                        if (*fptr == '\0') {
                            free(fline);
                            continue;
                        }
                        char *name = fptr;
                        while (*fptr && *fptr != ' ') fptr++;
                        if (*fptr) {
                            *fptr = '\0';
                            fptr++;
                        }
                        while (*fptr == ' ') fptr++;
                        char *note = fptr;
                        size_t nl = strlen(note);
                        if (nl > 0 && note[nl - 1] == '\r') {
                            note[nl - 1] = '\0';
                        }
                        if (strlen(name) > 0 && strlen(note) > 0) {
                            if (f_count >= f_cap) {
                                f_cap = f_cap == 0 ? 16 : f_cap * 2;
                                Note *nb = realloc(temp_store, f_cap * sizeof(Note));
                                if (nb) temp_store = nb;
                            }
                            temp_store[f_count].name = strdup(name);
                            temp_store[f_count].note = strdup(note);
                            f_count++;
                        } else {
                            success = 0;
                            free(fline);
                            break;
                        }
                        free(fline);
                    }
                    fclose(fp);
                    if (success) {
                        for (int i = 0; i < count; i++) {
                            free(store[i].name);
                            free(store[i].note);
                        }
                        free(store);
                        store = temp_store;
                        count = f_count;
                        cap = f_cap;
                    } else {
                        for (int i = 0; i < f_count; i++) {
                            free(temp_store[i].name);
                            free(temp_store[i].note);
                        }
                        free(temp_store);
                    }
                }
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Note *nb = realloc(store, cap * sizeof(Note));
                if (nb) store = nb;
            }
            store[count].name = strdup(name);
            store[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
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
