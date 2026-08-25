// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

Person *queue = NULL;
int count = 0;

int find_name(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char name[1000];
    char note[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            scanf("%s", name);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            queue = realloc(queue, sizeof(Person) * (count + 1));
            queue[count].name = strdup(name);
            queue[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (count > 0) {
                printf("%s\n", queue[0].name);
                free(queue[0].name);
                free(queue[0].note);
                for (int i = 0; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(queue[idx].name);
                free(queue[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                printf("%s\n", queue[idx].note);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);

    return 0;
}