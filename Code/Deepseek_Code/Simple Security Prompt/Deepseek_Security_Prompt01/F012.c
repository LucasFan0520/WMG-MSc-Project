// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Person;
Person *queue = NULL;
int q_count = 0, q_cap = 0;

int find_person(const char *name) {
    for (int i = 0; i < q_count; i++)
        if (strcmp(queue[i].name, name) == 0) return i;
    return -1;
}

void join_queue(const char *name, const char *note) {
    if (q_count == q_cap) {
        q_cap = q_cap ? q_cap * 2 : 4;
        queue = realloc(queue, sizeof(Person) * q_cap);
    }
    queue[q_count].name = strdup(name);
    queue[q_count].note = strdup(note);
    q_count++;
}

void serve(void) {
    if (q_count == 0) return;
    printf("%s %s\n", queue[0].name, queue[0].note);
    free(queue[0].name);
    free(queue[0].note);
    for (int i = 0; i < q_count - 1; i++) queue[i] = queue[i + 1];
    q_count--;
}

void cancel_person(const char *name) {
    int idx = find_person(name);
    if (idx < 0) return;
    free(queue[idx].name);
    free(queue[idx].note);
    for (int i = idx; i < q_count - 1; i++) queue[i] = queue[i + 1];
    q_count--;
}

void rejoin_queue(const char *name, const char *note) {
    cancel_person(name);
    join_queue(name, note);
}

void find_person_note(const char *name) {
    int idx = find_person(name);
    if (idx < 0) return;
    printf("%s\n", queue[idx].note);
}

void print_queue(void) {
    for (int i = 0; i < q_count; i++)
        printf("%s %s\n", queue[i].name, queue[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2) {
                if (strcmp(cmd, "JOIN") == 0) join_queue(name, note);
                else rejoin_queue(name, note);
            }
        } else if (strcmp(cmd, "SERVE") == 0) serve();
        else if (strcmp(cmd, "CANCEL") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) cancel_person(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_person_note(name);
        } else if (strcmp(cmd, "QUEUE") == 0) print_queue();
    }
    for (int i = 0; i < q_count; i++) { free(queue[i].name); free(queue[i].note); }
    free(queue);
    return 0;
}