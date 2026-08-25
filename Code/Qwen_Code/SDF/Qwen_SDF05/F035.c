// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

static Job *head = NULL, *tail = NULL;

static Job *create_job(char *name, char *payload) {
    Job *j = malloc(sizeof(Job));
    if (!j) return NULL;
    j->name = safe_strdup(name);
    j->payload = safe_strdup(payload);
    if (!j->name || !j->payload) {
        free(j->name); free(j->payload); free(j);
        return NULL;
    }
    j->next = NULL;
    return j;
}

static void cmd_enqueue(char *name, char *payload) {
    Job *j = create_job(name, payload);
    if (!j) return;
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

static void cmd_clone(char *oldName, char *newName) {
    for (Job *cur = head; cur; cur = cur->next) {
        if (strcmp(cur->name, oldName) == 0) {
            cmd_enqueue(newName, cur->payload);
            return;
        }
    }
}

static void cmd_cancel(char *name) {
    Job *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            if (cur == tail) tail = prev;
            free(cur->name); free(cur->payload); free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void cmd_run(char *name) {
    Job *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            printf("%s\n", cur->payload);
            if (prev) prev->next = cur->next;
            else head = cur->next;
            if (cur == tail) tail = prev;
            free(cur->name); free(cur->payload); free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

static void cmd_list(void) {
    for (Job *cur = head; cur; cur = cur->next)
        printf("%s %s\n", cur->name, cur->payload);
}

static void cleanup(void) {
    while (head) {
        Job *next = head->next;
        free(head->name); free(head->payload); free(head);
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
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256] = {0}; int n2 = 0;
            if (sscanf(rest, "%255s%n", name, &n2) < 1) continue;
            char *payload = rest + n2;
            while (*payload == ' ') payload++;
            cmd_enqueue(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char oldName[256] = {0}, newName[256] = {0};
            if (sscanf(rest, "%255s %255s", oldName, newName) < 2) continue;
            cmd_clone(oldName, newName);
        } else if (strcmp(cmd, "CANCEL") == 0 || strcmp(cmd, "RUN") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            if (strcmp(cmd, "CANCEL") == 0) cmd_cancel(name);
            else cmd_run(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            cmd_list();
        }
    }
    cleanup();
    return 0;
}