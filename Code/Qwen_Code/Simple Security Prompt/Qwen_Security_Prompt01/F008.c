// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stu {
    char *name;
    char *note;
    int score;
    struct Stu *next;
} Stu;

Stu *head = NULL;

void add_stu(const char *n, int s, const char *nt) {
    Stu *p = malloc(sizeof(Stu));
    p->name = strdup(n);
    p->score = s;
    p->note = strdup(nt);
    p->next = head;
    head = p;
}

void update_stu(const char *n, int s, const char *nt) {
    for (Stu *p = head; p; p = p->next) {
        if (strcmp(p->name, n) == 0) {
            p->score = s;
            free(p->note);
            p->note = strdup(nt);
            return;
        }
    }
}

void drop_stu(const char *n) {
    Stu *p = head;
    Stu *prev = NULL;
    while (p) {
        if (strcmp(p->name, n) == 0) {
            Stu *next = p->next;
            free(p->name);
            free(p->note);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void avg_stu() {
    int sum = 0;
    int cnt = 0;
    for (Stu *p = head; p; p = p->next) {
        sum += p->score;
        cnt++;
    }
    if (cnt > 0) {
        printf("%d\n", sum / cnt);
    }
}

void list_stu() {
    for (Stu *p = head; p; p = p->next) {
        printf("%s %d %s\n", p->name, p->score, p->note);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char name[1000];
        char note[8000];
        int s;
        note[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %d %[^\n]", name, &s, note);
            add_stu(name, s, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            sscanf(line, "%*s %s %d %[^\n]", name, &s, note);
            update_stu(name, s, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s", name);
            drop_stu(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            avg_stu();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_stu();
        }
    }
    while (head) {
        Stu *n = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = n;
    }
    return 0;
}