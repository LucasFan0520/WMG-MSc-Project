/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
    char *name;
    char *payload;
    struct Job *next;
} Job;

Job *head = NULL;
Job *tail = NULL;

Job *find_job(const char *name) {
    Job *j = head;
    while (j) {
        if (strcmp(j->name, name) == 0) return j;
        j = j->next;
    }
    return NULL;
}

void enqueue_job(const char *name, const char *payload) {
    if (find_job(name)) return;
    Job *j = malloc(sizeof(Job));
    if (!j) return;
    j->name = strdup(name);
    j->payload = strdup(payload);
    j->next = NULL;
    if (tail) tail->next = j;
    else head = j;
    tail = j;
}

void clone_job(const char *old, const char *new) {
    Job *j = find_job(old);
    if (!j || find_job(new)) return;
    enqueue_job(new, j->payload);
}

void cancel_job(const char *name) {
    Job *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->payload);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void run_job(const char *name) {
    Job *j = find_job(name);
    if (!j) return;
    printf("%s\n", j->payload);
    cancel_job(name);
}

void list_jobs(void) {
    Job *j = head;
    while (j) {
        printf("%s %s\n", j->name, j->payload);
        j = j->next;
    }
}

void free_all(void) {
    Job *j = head;
    while (j) {
        Job *tmp = j;
        j = j->next;
        free(tmp->name);
        free(tmp->payload);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *payload = p;
                while (*payload && *payload != ' ') payload++;
                while (*payload && *payload == ' ') payload++;
                if (*payload) {
                    enqueue_job(name, payload);
                }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new[256];
            if (sscanf(line + 6, "%255s %255s", old, new) == 2) {
                clone_job(old, new);
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            cancel_job(p);
        } else if (strcmp(cmd, "RUN") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            run_job(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_jobs();
        }
    }
    free_all();
    return 0;
}