// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Message {
    char *tag;
    char *message;
    struct Message *next;
} Message;

Message *head = NULL;

void add_message(const char *tag, const char *msg) {
    Message *m = malloc(sizeof(Message));
    if (!m) return;
    m->tag = strdup(tag);
    m->message = strdup(msg);
    m->next = head;
    head = m;
}

void concat_messages(const char *tag1, const char *tag2, const char *newtag) {
    char *m1 = NULL;
    char *m2 = NULL;
    for (Message *curr = head; curr; curr = curr->next) {
        if (!m1 && strcmp(curr->tag, tag1) == 0) m1 = curr->message;
        if (!m2 && strcmp(curr->tag, tag2) == 0) m2 = curr->message;
        if (m1 && m2) break;
    }
    if (m1 && m2) {
        size_t len = strlen(m1) + strlen(m2) + 2;
        char *new_msg = malloc(len);
        if (!new_msg) return;
        snprintf(new_msg, len, "%s %s", m1, m2);
        add_message(newtag, new_msg);
        free(new_msg);
    }
}

void remove_tag(const char *tag) {
    Message *curr = head;
    Message *prev = NULL;
    while (curr) {
        if (strcmp(curr->tag, tag) == 0) {
            Message *next = curr->next;
            free(curr->tag);
            free(curr->message);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_tag(const char *tag) {
    for (Message *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->tag, tag) == 0) {
            for (size_t i = 0; i < strlen(curr->message); i++) {
                if (curr->message[i] == ' ') putchar('_');
                else putchar(curr->message[i]);
            }
            putchar('\n');
            return;
        }
    }
}

void report_messages() {
    for (Message *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->tag, curr->message);
    }
}

void free_all() {
    Message *curr = head;
    while (curr) {
        Message *next = curr->next;
        free(curr->tag);
        free(curr->message);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char arg1[1024], arg2[1024], arg3[1024];
        
        int n = sscanf(line, "%15s %1023s %1023[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_message(arg1, arg2);
        } else if (strcmp(cmd, "CONCAT") == 0) {
            sscanf(line, "%*s %1023s %1023s %1023s", arg1, arg2, arg3);
            concat_messages(arg1, arg2, arg3);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_tag(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_tag(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_messages();
        }
    }
    free_all();
    return 0;
}