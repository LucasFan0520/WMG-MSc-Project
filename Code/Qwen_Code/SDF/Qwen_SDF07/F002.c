// F002.c
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

typedef struct { char *tag; char *msg; } TagMsg;
TagMsg *tm = NULL;
size_t tc = 0, tcap = 0;

void add_tm(const char *tag, const char *msg) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tm[i].tag, tag) == 0) {
            char *tmp = strdup(msg);
            if (!tmp) { perror("strdup"); exit(1); }
            free(tm[i].msg);
            tm[i].msg = tmp;
            return;
        }
    }
    if (tc == tcap) {
        size_t nc = tcap ? tcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(TagMsg)) { fprintf(stderr, "Overflow\n"); exit(1); }
        TagMsg *tmp = realloc(tm, nc * sizeof(TagMsg));
        if (!tmp) { perror("realloc"); exit(1); }
        tm = tmp;
        tcap = nc;
    }
    tm[tc].tag = strdup(tag);
    tm[tc].msg = strdup(msg);
    if (!tm[tc].tag || !tm[tc].msg) { perror("strdup"); exit(1); }
    tc++;
}

void concat_tm(const char *t1, const char *t2, const char *nt) {
    char *m1 = NULL, *m2 = NULL;
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tm[i].tag, t1) == 0) m1 = tm[i].msg;
        if (strcmp(tm[i].tag, t2) == 0) m2 = tm[i].msg;
    }
    if (!m1 || !m2) return;
    size_t l1 = strlen(m1), l2 = strlen(m2);
    if (l1 + l2 + 2 < l1) { fprintf(stderr, "Overflow\n"); exit(1); }
    char *buf = malloc(l1 + l2 + 2);
    if (!buf) { perror("malloc"); exit(1); }
    snprintf(buf, l1 + l2 + 2, "%s %s", m1, m2);
    add_tm(nt, buf);
    free(buf);
}

void remove_tm(const char *tag) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tm[i].tag, tag) == 0) {
            free(tm[i].tag); free(tm[i].msg);
            tm[i] = tm[--tc];
            break;
        }
    }
}

void find_tm(const char *tag) {
    for (size_t i = 0; i < tc; i++) {
        if (strcmp(tm[i].tag, tag) == 0) {
            for (size_t j = 0; tm[i].msg[j]; j++) putchar(tm[i].msg[j] == ' ' ? '_' : tm[i].msg[j]);
            putchar('\n');
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
        if (strcmp(cmd, "ADD") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) != 1) { free(line); continue; }
            char *msg = strchr(line, ' ');
            if (msg) { msg = strchr(msg + 1, ' '); if (msg) msg++; }
            if (!msg) msg = "";
            add_tm(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256], t2[256], nt[256];
            if (sscanf(line, "%*s %255s %255s %255s", t1, t2, nt) == 3) concat_tm(t1, t2, nt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) remove_tm(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(line, "%*s %255s", tag) == 1) find_tm(tag);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < tc; i++) printf("%s %s\n", tm[i].tag, tm[i].msg);
        }
        free(line);
    }
    for (size_t i = 0; i < tc; i++) { free(tm[i].tag); free(tm[i].msg); }
    free(tm);
    return 0;
}