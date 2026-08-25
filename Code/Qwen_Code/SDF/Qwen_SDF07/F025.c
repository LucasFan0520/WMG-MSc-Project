// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *name; char *note; } Stud;
typedef struct { char *name; Stud *studs; size_t sc; size_t scap; } Class;
Class *classes = NULL;
size_t cc = 0, ccap = 0;

Class* find_class(const char *name) {
    for (size_t i = 0; i < cc; i++) if (strcmp(classes[i].name, name) == 0) return &classes[i];
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    if (cc == ccap) {
        size_t nc = ccap ? ccap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Class)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Class *tmp = realloc(classes, nc * sizeof(Class));
        if (!tmp) { perror("realloc"); exit(1); }
        classes = tmp;
        ccap = nc;
    }
    classes[cc].name = strdup(name);
    classes[cc].studs = NULL;
    classes[cc].sc = 0;
    classes[cc].scap = 0;
    if (!classes[cc].name) { perror("strdup"); exit(1); }
    cc++;
}

void add_stud(Class *c, const char *name, const char *note) {
    if (c->sc == c->scap) {
        size_t nc = c->scap ? c->scap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Stud)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Stud *tmp = realloc(c->studs, nc * sizeof(Stud));
        if (!tmp) { perror("realloc"); exit(1); }
        c->studs = tmp;
        c->scap = nc;
    }
    c->studs[c->sc].name = strdup(name);
    c->studs[c->sc].note = strdup(note);
    if (!c->studs[c->sc].name || !c->studs[c->sc].note) { perror("strdup"); exit(1); }
    c->sc++;
}

void drop_stud(Class *c, const char *name) {
    for (size_t i = 0; i < c->sc; i++) {
        if (strcmp(c->studs[i].name, name) == 0) {
            free(c->studs[i].name); free(c->studs[i].note);
            c->studs[i] = c->studs[--c->sc];
            return;
        }
    }
}

void del_class(const char *name) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            free(classes[i].name);
            for (size_t j = 0; j < classes[i].sc; j++) { free(classes[i].studs[j].name); free(classes[i].studs[j].note); }
            free(classes[i].studs);
            classes[i] = classes[--cc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) add_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256], name[256];
            if (sscanf(line, "%*s %255s %255s", cls, name) == 2) {
                Class *c = find_class(cls);
                if (c) {
                    char *note = strchr(line, ' ');
                    if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
                    if (!note) note = "";
                    add_stud(c, name, note);
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char fc[256], tc[256], name[256];
            if (sscanf(line, "%*s %255s %255s %255s", fc, tc, name) == 3) {
                Class *src = find_class(fc), *dst = find_class(tc);
                if (src && dst) {
                    for (size_t i = 0; i < src->sc; i++) {
                        if (strcmp(src->studs[i].name, name) == 0) {
                            add_stud(dst, src->studs[i].name, src->studs[i].note);
                            free(src->studs[i].name); free(src->studs[i].note);
                            src->studs[i] = src->studs[--src->sc];
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256], name[256];
            if (sscanf(line, "%*s %255s %255s", cls, name) == 2) {
                Class *c = find_class(cls);
                if (c) drop_stud(c, name);
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < cc; i++) {
                printf("%s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].sc; j++) {
                    printf(" %s %s\n", classes[i].studs[j].name, classes[i].studs[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < cc; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].sc; j++) { free(classes[i].studs[j].name); free(classes[i].studs[j].note); }
        free(classes[i].studs);
    }
    free(classes);
    return 0;
}