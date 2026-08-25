// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
int count = 0;

Stop* get_at(int idx) {
    Stop *p = head;
    for (int i = 0; i < idx && p; i++) p = p->next;
    return p;
}

int main() {
    char cmd[20];
    char name[100];
    char note[1000];
    int idx, from, to;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name);
            s->note = strdup(note);
            s->next = NULL;
            if (!head) head = s;
            else {
                Stop *p = head;
                while (p->next) p = p->next;
                p->next = s;
            }
            count++;
        } else if (!strcmp(cmd, "INSERT")) {
            scanf("%d %s", &idx, name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (idx < 0 || idx > count) continue;
            Stop *s = malloc(sizeof(Stop));
            s->name = strdup(name);
            s->note = strdup(note);
            if (idx == 0) {
                s->next = head;
                head = s;
            } else {
                Stop *p = get_at(idx - 1);
                s->next = p->next;
                p->next = s;
            }
            count++;
        } else if (!strcmp(cmd, "MOVE")) {
            scanf("%d %d", &from, &to);
            if (from < 0 || from >= count || to < 0 || to >= count || from == to) continue;
            Stop *p = head;
            Stop *prev = NULL;
            for (int i = 0; i < from; i++) {
                prev = p;
                p = p->next;
            }
            if (prev) prev->next = p->next;
            else head = p->next;
            p->next = NULL;
            if (to == 0) {
                p->next = head;
                head = p;
            } else {
                Stop *q = get_at(to - 1);
                p->next = q->next;
                q->next = p;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%d", &idx);
            if (idx < 0 || idx >= count) continue;
            Stop *p = head;
            Stop *prev = NULL;
            for (int i = 0; i < idx; i++) {
                prev = p;
                p = p->next;
            }
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name);
            free(p->note);
            free(p);
            count--;
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            for (Stop *s = head; s; s = s->next) {
                if (!strcmp(s->name, name)) {
                    printf("%s\n", s->note);
                    break;
                }
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (Stop *s = head; s; s = s->next) {
                printf("%s %s\n", s->name, s->note);
            }
        }
    }
    return 0;
}