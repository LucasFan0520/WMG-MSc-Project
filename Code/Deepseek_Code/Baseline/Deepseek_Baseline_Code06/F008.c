/* F008.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *head = NULL;

Student* find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = head;
    head = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *s = head, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score() {
    Student *s = head;
    int total = 0, count = 0;
    while (s) {
        total += s->score;
        count++;
        s = s->next;
    }
    if (count == 0) printf("0\n");
    else printf("%d\n", total / count);
}

void list_students() {
    Student *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

void free_all() {
    while (head) {
        Student *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256]; int score; char note[4096];
        int n = sscanf(line, "%31s %255s %d %[^\n]", cmd, name, &score, note);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0 && n == 4) {
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0 && n == 2) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0 && n == 1) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_students();
        }
    }
    free_all();
    return 0;
}