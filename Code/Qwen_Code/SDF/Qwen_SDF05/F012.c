// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct QNode {
    char *name;
    char *note;
    struct QNode *next;
} QNode;

static QNode *head = NULL, *tail = NULL;

static QNode *create_node(char *name, char *note) {
    QNode *n = malloc(sizeof(QNode));
    if (!n) return NULL;
    n->name = safe_strdup(name);
    n->note = safe_strdup(note);
    if (!n->name || !n->note) {
        free(n->name); free(n->note); free(n);
        return NULL;
    }
    n->next = NULL;
    return n;
}

static void cmd_join(char *name, char *note) {
    QNode *n = create_node(name, note);
    if (!n) return;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

static void cmd_serve(void) {
    if (!head) return;
    printf("%s\n", head->name);
    QNode *old = head;
    head = head->next;
    if (!head) tail = NULL;
    free(old->name); free(old->note); free(old);
}

static void cmd_cancel(char *name) {
    QNode *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            if (cur == tail) tail = prev;
            free(cur->name); free(cur->note); free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void cmd_rejoin(char *name, char *note) {
    cmd_cancel(name);
    cmd_join(name, note);
}

static void cmd_find(char *name) {
    for (QNode *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->name, name) == 0) {
            printf("%s\n", cur->note);
            return;
        }
    }
}

static void cmd_queue(void) {
    for (QNode *cur = head; cur; cur = cur->next)
        printf("%s %s\n", cur->name, cur->note);
}

static void cleanup(void) {
    while (head) {
        QNode *next = head->next;
        free(head->name); free(head->note); free(head);
        head = next;
    }
    tail = NULL;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *note = rest + n2;
            while (*note == ' ') note++;
            if (strcmp(cmd, "JOIN") == 0) cmd_join(name, note);
            else cmd_rejoin(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            cmd_serve();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_cancel(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_find(name);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            cmd_queue();
        }
    }
    cleanup();
    return 0;
}