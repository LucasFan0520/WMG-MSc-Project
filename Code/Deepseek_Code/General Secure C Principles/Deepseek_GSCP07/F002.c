// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Msg {
    char *tag;
    char *message;
    struct Msg *next;
} Msg;

Msg *head = NULL;

Msg* find_first(const char *tag) {
    Msg *m = head;
    while (m) {
        if (strcmp(m->tag, tag) == 0) return m;
        m = m->next;
    }
    return NULL;
}

void free_msg(Msg *m) {
    free(m->tag);
    free(m->message);
    free(m);
}

void remove_all(const char *tag) {
    Msg *m = head, *prev = NULL;
    while (m) {
        if (strcmp(m->tag, tag) == 0) {
            Msg *tofree = m;
            if (prev) prev->next = m->next;
            else head = m->next;
            m = m->next;
            free_msg(tofree);
        } else {
            prev = m;
            m = m->next;
        }
    }
}

void add_msg(const char *tag, const char *msg) {
    Msg *new = malloc(sizeof(Msg));
    new->tag = strdup(tag);
    new->message = strdup(msg);
    new->next = NULL;
    if (!head) head = new;
    else {
        Msg *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_add(char *cmd) {
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    char *msg = strtok(NULL, "");
    if (!msg) msg = "";
    add_msg(tag, msg);
}

void handle_concat(char *cmd) {
    char *tag1 = strtok(NULL, " ");
    char *tag2 = strtok(NULL, " ");
    char *newtag = strtok(NULL, " ");
    if (!tag1 || !tag2 || !newtag) return;
    Msg *m1 = find_first(tag1);
    Msg *m2 = find_first(tag2);
    if (!m1 || !m2) return;
    size_t len = strlen(m1->message) + strlen(m2->message) + 2;
    char *combined = malloc(len);
    sprintf(combined, "%s %s", m1->message, m2->message);
    add_msg(newtag, combined);
    free(combined);
}

void handle_remove(char *cmd) {
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    remove_all(tag);
}

void handle_find(char *cmd) {
    char *tag = strtok(NULL, " ");
    if (!tag) return;
    Msg *m = find_first(tag);
    if (!m) return;
    for (char *s = m->message; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void report() {
    Msg *m = head;
    while (m) {
        printf("%s: %s\n", m->tag, m->message);
        m = m->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "CONCAT") == 0) handle_concat(cmd);
        else if (strcmp(cmd, "REMOVE") == 0) handle_remove(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "REPORT") == 0) report();
    }
    while (head) {
        Msg *tmp = head;
        head = head->next;
        free_msg(tmp);
    }
    return 0;
}