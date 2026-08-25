// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stu {
    char *name;
    int score;
    char *note;
    struct stu *next;
} Stu;

Stu *head = NULL;
Stu *tail = NULL;

Stu *find_stu(const char *name) {
    Stu *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_stu(const char *name, int score, const char *note) {
    if (find_stu(name)) return;
    Stu *s = malloc(sizeof(Stu));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
}

void update_stu(const char *name, int score, const char *note) {
    Stu *s = find_stu(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_stu(const char *name) {
    Stu *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            if (s == tail) tail = prev;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    int total = 0, n = 0;
    Stu *s = head;
    while (s) { total += s->score; n++; s = s->next; }
    printf("%d\n", n ? total / n : 0);
}

void list_stu(void) {
    Stu *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        int score;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %[^\n]", name, &score, note) == 3)
                add_stu(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d %[^\n]", name, &score, note) == 3)
                update_stu(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                drop_stu(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_stu();
        }
    }
    return 0;
}