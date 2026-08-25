// filename: F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MsgNode {
    char *tag;
    char *message;
    struct MsgNode *next;
} MsgNode;

static MsgNode *head = NULL;

static void cleanup(void) {
    while (head) {
        MsgNode *tmp = head;
        head = head->next;
        free(tmp->tag);
        free(tmp->message);
        free(tmp);
    }
}

static MsgNode *find_tag(const char *tag) {
    for (MsgNode *n = head; n; n = n->next)
        if (strcmp(n->tag, tag) == 0)
            return n;
    return NULL;
}

static void add_message(const char *tag, const char *msg) {
    MsgNode *n = malloc(sizeof(MsgNode));
    if (!n) return;
    n->tag = strdup(tag);
    n->message = strdup(msg);
    if (!n->tag || !n->message) {
        free(n->tag);
        free(n->message);
        free(n);
        return;
    }
    n->next = head;
    head = n;
}

static void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    MsgNode *n1 = find_tag(tag1);
    MsgNode *n2 = find_tag(tag2);
    if (!n1 || !n2) return;
    size_t len = strlen(n1->message) + strlen(n2->message) + 2;
    char *joined = malloc(len);
    if (!joined) return;
    snprintf(joined, len, "%s %s", n1->message, n2->message);
    add_message(newtag, joined);
    free(joined);
}

static void remove_tag(const char *tag) {
    MsgNode *prev = NULL;
    for (MsgNode *n = head; n; n = n->next) {
        if (strcmp(n->tag, tag) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->tag);
            free(n->message);
            free(n);
            n = prev ? prev->next : head;
            if (!n) break;
        }
        prev = n;
    }
}

static void find_tag_print(const char *tag) {
    MsgNode *n = find_tag(tag);
    if (!n) return;
    for (char *p = n->message; *p; ++p)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void report(void) {
    for (MsgNode *n = head; n; n = n->next)
        printf("%s: %s\n", n->tag, n->message);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_message(arg1, arg2);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            if (*arg1 && *arg2 && *arg3) concat_messages(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_tag_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}