// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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
} NoteRecord;

int main(int argc, char **argv) {
    NoteRecord *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *filepath = (argc > 1) ? argv[1] : NULL;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LOAD") == 0) {
                if (filepath) {
                    FILE *fp = fopen(filepath, "r");
                    if (fp) {
                        NoteRecord *t_arr = NULL;
                        size_t t_count = 0;
                        size_t t_cap = 0;
                        int ok = 1;
                        while (1) {
                            char *fline = read_line(fp);
                            if (!fline) break;
                            char *p = strchr(fline, ' ');
                            if (p) {
                                *p = '\0';
                                if (t_count >= t_cap) {
                                    size_t ncap = t_cap == 0 ? 4 : t_cap * 2;
                                    NoteRecord *narr = realloc(t_arr, ncap * sizeof(NoteRecord));
                                    if (narr) { t_arr = narr; t_cap = ncap; }
                                }
                                if (t_count < t_cap) {
                                    t_arr[t_count].name = strdup(fline);
                                    t_arr[t_count].note = strdup(p + 1);
                                    t_count++;
                                }
                            } else {
                                if (strcmp(fline, "") != 0) {
                                    ok = 0;
                                }
                            }
                            free(fline);
                        }
                        fclose(fp);
                        if (ok) {
                            for (size_t i = 0; i < count; i++) {
                                free(arr[i].name);
                                free(arr[i].note);
                            }
                            free(arr);
                            arr = t_arr;
                            count = t_count;
                            cap = t_cap;
                        } else {
                            for (size_t i = 0; i < t_count; i++) {
                                free(t_arr[i].name);
                                free(t_arr[i].note);
                            }
                            free(t_arr);
                        }
                    }
                }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].name, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = args;
            char *p2 = strchr(name, ' ');
            if (p2) {
                *p2 = '\0';
                char *note = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    NoteRecord *narr = realloc(arr, ncap * sizeof(NoteRecord));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].name = strdup(name);
                    arr[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
