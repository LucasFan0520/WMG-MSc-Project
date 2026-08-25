// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct msg {
    char *tag;
    char *message;
    struct msg *next;
} Msg;

Msg *head = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Msg *find_first(const char *tag) {
    Msg *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void add_msg(const char *tag, const char *msg) {
    Msg *newm = malloc(sizeof(Msg));
    if (!newm) { fprintf(stderr, "Memory error\n"); exit(1); }
    newm->tag = strdup(tag);
    newm->message = strdup(msg);
    if (!newm->tag || !newm->message) { fprintf(stderr, "Memory error\n"); exit(1); }
    newm->next = head;
    head = newm;
}

void concat_msgs(const char *tag1, const char *tag2, const char *newtag) {
    Msg *m1 = find_first(tag1);
    Msg *m2 = find_first(tag2);
    if (!m1 || !m2) return;
    size_t len = strlen(m1->message) + strlen(m2->message) + 2;
    char *combined = malloc(len);
    if (!combined) { fprintf(stderr, "Memory error\n"); exit(1); }
    snprintf(combined, len, "%s %s", m1->message, m2->message);
    add_msg(newtag, combined);
    free(combined);
}

void remove_tag(const char *tag) {
    Msg *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            if (prev) prev->next = m->next;
            else head = m->next;
            free(m->tag);
            free(m->message);
            free(m);
            m = prev ? prev->next : head;
        } else {
            prev = m;
            m = m->next;
        }
    }
}

void find_tag(const char *tag) {
    Msg *m = find_first(tag);
    if (!m) return;
    for (char *s = m->message; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void report_all(void) {
    Msg *m = head;
    while (m) {
        printf("%s ", m->tag);
        for (char *s = m->message; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        m = m->next;
    }
}

void free_all(void) {
    Msg *m = head;
    while (m) {
        Msg *next = m->next;
        free(m->tag);
        free(m->message);
        free(m);
        m = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "REPORT") == 0) { report_all(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            add_msg(tag, rest);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *tag2 = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *newtag = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            concat_msgs(tag1, tag2, newtag);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_tag(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_tag(rest);
        }
    }
    free_all();
    return 0;
}