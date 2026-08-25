// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000

typedef struct {
    char name[MAX_NAME];
    char note[MAX_NOTE];
} Person;

Person queue[10000];
int q_count = 0;

int find_person(const char *name) {
    for (int i = 0; i < q_count; i++) {
        if (strcmp(queue[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], note[MAX_NOTE];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "JOIN") == 0) {
            scanf("%s", name);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            strcpy(queue[q_count].name, name);
            strcpy(queue[q_count].note, note);
            q_count++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (q_count > 0) {
                printf("%s\n", queue[0].name);
                for (int i = 0; i < q_count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                q_count--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) {
                for (int i = idx; i < q_count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                q_count--;
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            scanf("%s", name);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            strcpy(queue[q_count].name, name);
            strcpy(queue[q_count].note, note);
            q_count++;
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_person(name);
            if (idx != -1) {
                printf("%s\n", queue[idx].note);
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < q_count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }
    
    return 0;
}