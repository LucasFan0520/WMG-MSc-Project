// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct MsgNode {
    char *tag;
    char *message;
    struct MsgNode *next;
} MsgNode;

MsgNode *head = NULL;

void add_msg(const char *tag, const char *msg) {
    MsgNode *new = malloc(sizeof(MsgNode));
    new->tag = strdup(tag);
    new->message = strdup(msg);
    new->next = NULL;
    if (!head) head = new;
    else {
        MsgNode *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void remove_tag(const char *tag) {
    MsgNode *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) {
            MsgNode *tmp = cur;
            if (prev) prev->next = cur->next;
            else head = cur->next;
            cur = cur->next;
            free(tmp->tag);
            free(tmp->message);
            free(tmp);
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void free_all() {
    MsgNode *cur = head;
    while (cur) {
        MsgNode *tmp = cur;
        cur = cur->next;
        free(tmp->tag);
        free(tmp->message);
        free(tmp);
    }
}

char* first_msg_for_tag(const char *tag) {
    MsgNode *cur = head;
    while (cur) {
        if (strcmp(cur->tag, tag) == 0) return cur->message;
        cur = cur->next;
    }
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = strtok(NULL, " ");
            if (!tag) continue;
            char *msg = strtok(NULL, "");
            if (!msg) msg = "";
            add_msg(tag, msg);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = strtok(NULL, " ");
            if (!tag1) continue;
            char *tag2 = strtok(NULL, " ");
            if (!tag2) continue;
            char *newtag = strtok(NULL, " ");
            if (!newtag) continue;
            char *m1 = first_msg_for_tag(tag1);
            char *m2 = first_msg_for_tag(tag2);
            if (!m1 || !m2) continue;
            char *combined = malloc(strlen(m1) + strlen(m2) + 2);
            sprintf(combined, "%s %s", m1, m2);
            add_msg(newtag, combined);
            free(combined);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = strtok(NULL, " ");
            if (tag) remove_tag(tag);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = strtok(NULL, " ");
            if (!tag) continue;
            char *msg = first_msg_for_tag(tag);
            if (!msg) continue;
            while (*msg) {
                if (*msg == ' ') putchar('_');
                else putchar(*msg);
                msg++;
            }
            putchar('\n');
        } else if (strcmp(cmd, "REPORT") == 0) {
            MsgNode *cur = head;
            while (cur) {
                printf("%s %s\n", cur->tag, cur->message);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}