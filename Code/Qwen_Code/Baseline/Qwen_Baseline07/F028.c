// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name, *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
int count = 0;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Stop *get_at(int idx) {
    Stop *curr = head;
    for (int i = 0; i < idx; i++) curr = curr->next;
    return curr;
}

int main() {
    char cmd[20], name[256], note[1024];
    int idx1, idx2;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name); read_rest(note, sizeof(note));
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name); s->note = strdup(note); s->next = NULL;
            if (!head) head = s; else { Stop *c = head; while (c->next) c = c->next; c->next = s; }
            count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d %s", &idx1, name); read_rest(note, sizeof(note));
            if (idx1 < 0 || idx1 > count) continue;
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name); s->note = strdup(note);
            if (idx1 == 0) { s->next = head; head = s; }
            else { Stop *prev = get_at(idx1 - 1); s->next = prev->next; prev->next = s; }
            count++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            scanf("%d %d", &idx1, &idx2);
            if (idx1 < 0 || idx1 >= count || idx2 < 0 || idx2 >= count) continue;
            Stop *prev1 = (idx1 == 0) ? NULL : get_at(idx1 - 1);
            Stop *curr1 = prev1 ? prev1->next : head;
            if (prev1) prev1->next = curr1->next; else head = curr1->next;
            curr1->next = NULL;
            if (idx2 == 0) { curr1->next = head; head = curr1; }
            else {
                Stop *prev2 = get_at(idx2 - 1);
                curr1->next = prev2->next; prev2->next = curr1;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%d", &idx1);
            if (idx1 < 0 || idx1 >= count) continue;
            Stop *prev = (idx1 == 0) ? NULL : get_at(idx1 - 1);
            Stop *curr = prev ? prev->next : head;
            if (prev) prev->next = curr->next; else head = curr->next;
            free(curr->name); free(curr->note); free(curr);
            count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            for (Stop *s = head; s; s = s->next) if (strcmp(s->name, name) == 0) { printf("%s\n", s->note); break; }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Stop *s = head; s; s = s->next) printf("%s %s\n", s->name, s->note);
        }
    }
    return 0;
}