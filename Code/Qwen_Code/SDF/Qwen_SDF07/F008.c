// F008.c
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

typedef struct { char *name; int score; char *note; } Stu;
Stu *stus = NULL;
size_t sc = 0, scap = 0;

void add_stu(const char *name, int score, const char *note) {
    for (size_t i = 0; i < sc; i++) {
        if (strcmp(stus[i].name, name) == 0) {
            stus[i].score = score;
            char *tmp = strdup(note);
            if (!tmp) { perror("strdup"); exit(1); }
            free(stus[i].note);
            stus[i].note = tmp;
            return;
        }
    }
    if (sc == scap) {
        size_t nc = scap ? scap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Stu)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Stu *tmp = realloc(stus, nc * sizeof(Stu));
        if (!tmp) { perror("realloc"); exit(1); }
        stus = tmp;
        scap = nc;
    }
    stus[sc].name = strdup(name);
    stus[sc].score = score;
    stus[sc].note = strdup(note);
    if (!stus[sc].name || !stus[sc].note) { perror("strdup"); exit(1); }
    sc++;
}

void drop_stu(const char *name) {
    for (size_t i = 0; i < sc; i++) {
        if (strcmp(stus[i].name, name) == 0) {
            free(stus[i].name); free(stus[i].note);
            stus[i] = stus[--sc];
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
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char name[256]; int score;
            if (sscanf(line, "%*s %255s %d", name, &score) != 2) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_stu(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) drop_stu(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (sc == 0) { printf("0\n"); }
            else {
                long sum = 0;
                for (size_t i = 0; i < sc; i++) sum += stus[i].score;
                printf("%ld\n", sum / (long)sc);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < sc; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < sc; i++) { free(stus[i].name); free(stus[i].note); }
    free(stus);
    return 0;
}