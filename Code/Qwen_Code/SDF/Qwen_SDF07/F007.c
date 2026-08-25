// F007.c
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

typedef struct { char *time; char *label; char *note; } Act;
Act *acts = NULL;
size_t ac = 0, acap = 0;

void add_act(const char *time, const char *label, const char *note) {
    if (ac == acap) {
        size_t nc = acap ? acap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Act)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Act *tmp = realloc(acts, nc * sizeof(Act));
        if (!tmp) { perror("realloc"); exit(1); }
        acts = tmp;
        acap = nc;
    }
    acts[ac].time = strdup(time);
    acts[ac].label = strdup(label);
    acts[ac].note = strdup(note);
    if (!acts[ac].time || !acts[ac].label || !acts[ac].note) { perror("strdup"); exit(1); }
    ac++;
}

void del_act(const char *time, const char *label) {
    for (size_t i = 0; i < ac; ) {
        if (strcmp(acts[i].time, time) == 0 && strcmp(acts[i].label, label) == 0) {
            free(acts[i].time); free(acts[i].label); free(acts[i].note);
            acts[i] = acts[--ac];
        } else i++;
    }
}

int cmp_act(const void *a, const void *b) {
    const Act *aa = a, *bb = b;
    int c = strcmp(aa->time, bb->time);
    if (c != 0) return c;
    return strcmp(aa->label, bb->label);
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char time[256], label[256];
            if (sscanf(line, "%*s %255s %255s", time, label) != 2) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_act(time, label, note);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time[256], label[256];
            if (sscanf(line, "%*s %255s %255s", time, label) == 2) del_act(time, label);
        } else if (strcmp(cmd, "FIND") == 0) {
            char label[256];
            if (sscanf(line, "%*s %255s", label) == 1) {
                Act *earliest = NULL;
                for (size_t i = 0; i < ac; i++) {
                    if (strcmp(acts[i].label, label) == 0) {
                        if (!earliest || strcmp(acts[i].time, earliest->time) < 0) earliest = &acts[i];
                    }
                }
                if (earliest) printf("%s %s %s\n", earliest->time, earliest->label, earliest->note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            qsort(acts, ac, sizeof(Act), cmp_act);
            for (size_t i = 0; i < ac; i++) printf("%s %s %s\n", acts[i].time, acts[i].label, acts[i].note);
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < ac; i++) { free(acts[i].time); free(acts[i].label); free(acts[i].note); }
            ac = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < ac; i++) { free(acts[i].time); free(acts[i].label); free(acts[i].note); }
    free(acts);
    return 0;
}