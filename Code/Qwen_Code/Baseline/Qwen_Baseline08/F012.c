// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_Q 10000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char note[MAX_LEN];
} Person;

Person q[MAX_Q];
int head = 0, tail = 0;

int find_person(const char *name) {
    for (int i = head; i < tail; i++) if (strcmp(q[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "JOIN") == 0) {
            scanf("%s", name);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (tail < MAX_Q) {
                strcpy(q[tail].name, name);
                strcpy(q[tail].note, note);
                tail++;
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head < tail) {
                printf("%s\n", q[head].name);
                head++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) {
                for (int i = idx; i < tail - 1; i++) q[i] = q[i + 1];
                tail--;
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            scanf("%s", name);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find_person(name);
            if (idx == -1 && tail < MAX_Q) {
                strcpy(q[tail].name, name);
                strcpy(q[tail].note, note);
                tail++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) printf("%s\n", q[idx].note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = head; i < tail; i++) printf("%s %s\n", q[i].name, q[i].note);
        }
    }
    return 0;
}