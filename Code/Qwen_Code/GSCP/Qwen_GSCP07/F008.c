// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stu { char *name, *note; int score; struct Stu *next; } Stu;
Stu *head = NULL;

void add_stu(const char *name, int score, const char *note) {
    for (Stu *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            s->score = score;
            free(s->note);
            s->note = strdup(note);
            return;
        }
    }
    Stu *s = malloc(sizeof(Stu));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = head;
    head = s;
}

void update_stu(const char *name, int score, const char *note) { add_stu(name, score, note); }

void drop_stu(const char *name) {
    Stu **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Stu *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void average() {
    int sum = 0, n = 0;
    for (Stu *s = head; s; s = s->next) { sum += s->score; n++; }
    if (n > 0) printf("%d\n", sum / n);
    else printf("0\n");
}

void list_stu() {
    for (Stu *s = head; s; s = s->next) printf("%s %d %s\n", s->name, s->score, s->note);
}

void cleanup() {
    while (head) {
        Stu *tmp = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], note[8192] = "";
        int score;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &score) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) strcpy(note, t + 1); } }
                add_stu(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) drop_stu(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) average();
        else if (strcmp(cmd, "LIST") == 0) list_stu();
    }
    cleanup();
    return 0;
}