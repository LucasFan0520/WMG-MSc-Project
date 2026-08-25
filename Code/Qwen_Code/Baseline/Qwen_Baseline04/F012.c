// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

Person *queue = NULL;
int qcount = 0;

int find_person(const char *name) {
    for (int i = 0; i < qcount; i++) {
        if (strcmp(queue[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "JOIN") == 0) {
            char name[100], note[1000];
            scanf("%s", name);
            scanf(" %[^\n]", note);
            queue = realloc(queue, sizeof(Person) * (qcount + 1));
            queue[qcount].name = strdup(name);
            queue[qcount].note = strdup(note);
            qcount++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (qcount > 0) {
                printf("%s\n", queue[0].name);
                free(queue[0].name);
                free(queue[0].note);
                for (int i = 0; i < qcount - 1; i++) queue[i] = queue[i+1];
                qcount--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) {
                free(queue[idx].name);
                free(queue[idx].note);
                for (int i = idx; i < qcount - 1; i++) queue[i] = queue[i+1];
                qcount--;
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            char name[100], note[1000];
            scanf("%s", name);
            scanf(" %[^\n]", note);
            queue = realloc(queue, sizeof(Person) * (qcount + 1));
            queue[qcount].name = strdup(name);
            queue[qcount].note = strdup(note);
            qcount++;
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) {
                printf("%s\n", queue[idx].note);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < qcount; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }
    for (int i = 0; i < qcount; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}