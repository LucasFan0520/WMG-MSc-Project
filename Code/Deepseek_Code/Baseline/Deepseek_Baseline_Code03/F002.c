// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct message {
    char *msg;
    struct message *next;
};

struct tag {
    char *tag;
    struct message *msgs;
    struct tag *next;
};

struct tag *tags = NULL;

struct tag *find_tag(char *tag) {
    struct tag *t = tags;
    while (t) {
        if (strcmp(t->tag, tag) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_tag_message(char *tag, char *msg) {
    struct tag *t = find_tag(tag);
    if (!t) {
        t = malloc(sizeof(struct tag));
        t->tag = malloc(strlen(tag) + 1);
        strcpy(t->tag, tag);
        t->msgs = NULL;
        t->next = tags;
        tags = t;
    }
    struct message *m = malloc(sizeof(struct message));
    m->msg = malloc(strlen(msg) + 1);
    strcpy(m->msg, msg);
    m->next = t->msgs;
    t->msgs = m;
}

void concat_tags(char *tag1, char *tag2, char *newtag) {
    struct tag *t1 = find_tag(tag1);
    struct tag *t2 = find_tag(tag2);
    if (!t1 || !t2) return;
    char *first1 = t1->msgs ? t1->msgs->msg : "";
    char *first2 = t2->msgs ? t2->msgs->msg : "";
    char *combined = malloc(strlen(first1) + strlen(first2) + 2);
    sprintf(combined, "%s %s", first1, first2);
    add_tag_message(newtag, combined);
    free(combined);
}

void remove_tag(char *tag) {
    struct tag *t = find_tag(tag);
    if (!t) return;
    // delete all messages
    struct message *m = t->msgs;
    while (m) {
        struct message *next = m->next;
        free(m->msg);
        free(m);
        m = next;
    }
    // remove from list
    struct tag *prev = NULL;
    struct tag *cur = tags;
    while (cur) {
        if (cur == t) {
            if (prev) prev->next = cur->next;
            else tags = cur->next;
            free(cur->tag);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void find_tag_print(char *tag) {
    struct tag *t = find_tag(tag);
    if (t && t->msgs) {
        char *msg = t->msgs->msg;
        for (char *c = msg; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
    }
}

void report() {
    struct tag *t = tags;
    while (t) {
        struct message *m = t->msgs;
        while (m) {
            printf("%s %s\n", t->tag, m->msg);
            m = m->next;
        }
        t = t->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[256];
        int n = sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_tag_message(arg1, rest);
        } else if (strcmp(cmd, "CONCAT") == 0 && n >= 4) {
            concat_tags(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_tag_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    // cleanup omitted for brevity
    return 0;
}