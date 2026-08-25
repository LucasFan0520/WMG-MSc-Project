// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

Person q[10000];
int head = 0, tail = 0;

int find_name(char *name) {
    for (int i = head; i < tail; i++) if (strcmp(q[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *note = rest + strlen(name);
            while (*note == ' ') note++;
            q[tail].name = strdup(name);
            q[tail].note = strdup(note);
            tail++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (head < tail) {
                printf("%s\n", q[head].name);
                free(q[head].name);
                free(q[head].note);
                head++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(q[idx].name);
                free(q[idx].note);
                for (int i = idx; i < tail - 1; i++) q[i] = q[i+1];
                tail--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) printf("%s\n", q[idx].note);
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = head; i < tail; i++) printf("%s\n", q[i].name);
        }
    }
    for (int i = head; i < tail; i++) {
        free(q[i].name);
        free(q[i].note);
    }
    return 0;
}