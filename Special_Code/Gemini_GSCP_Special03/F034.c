// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line_from(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
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
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} NoteStore;

int main(int argc, char **argv) {
    NoteStore *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    char *filepath = argc > 1 ? argv[1] : NULL;
    while (1) {
        char *line = read_line_from(stdin);
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
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) {
                FILE *fp = fopen(filepath, "r");
                if (fp) {
                    NoteStore *n_items = NULL;
                    size_t n_count = 0;
                    size_t n_cap = 0;
                    int success = 1;
                    while (1) {
                        char *fline = read_line_from(fp);
                        if (!fline) break;
                        char *fp_line = fline;
                        while (*fp_line == ' ') fp_line++;
                        if (*fp_line != '\0') {
                            char *rname = fp_line;
                            while (*fp_line && *fp_line != ' ') fp_line++;
                            if (*fp_line) {
                                *fp_line = '\0';
                                fp_line++;
                                while (*fp_line == ' ') fp_line++;
                            }
                            char *rnote = fp_line;
                            size_t rnlen = strlen(rnote);
                            while (rnlen > 0 && (rnote[rnlen-1] == '\n' || rnote[rnlen-1] == '\n')) {
                                rnote[rnlen-1] = '\0';
                                rnlen--;
                            }
                            if (strlen(rname) > 0) {
                                if (n_count >= n_cap) {
                                    n_cap = n_cap == 0 ? 8 : n_cap * 2;
                                    NoteStore *tmp = realloc(n_items, n_cap * sizeof(NoteStore));
                                    if (tmp) n_items = tmp;
                                }
                                n_items[n_count].name = mystrdup(rname);
                                n_items[n_count].note = mystrdup(rnote);
                                n_count++;
                            } else {
                                success = 0;
                            }
                        }
                        free(fline);
                    }
                    fclose(fp);
                    if (success) {
                        for (size_t i = 0; i < count; i++) {
                            free(items[i].name);
                            free(items[i].note);
                        }
                        free(items);
                        items = n_items;
                        count = n_count;
                        cap = n_cap;
                    } else {
                        for (size_t i = 0; i < n_count; i++) {
                            free(n_items[i].name);
                            free(n_items[i].note);
                        }
                        free(n_items);
                    }
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                NoteStore *nitems = realloc(items, cap * sizeof(NoteStore));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].note = mystrdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}
