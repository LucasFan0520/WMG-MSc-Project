// F008.c
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
int student_count = 0;

Student* find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_student(Student *s) {
    free(s->name);
    free(s->note);
    free(s);
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *new = malloc(sizeof(Student));
    new->name = strdup(name);
    new->score = score;
    new->note = strdup(note);
    new->next = NULL;
    if (!head) head = new;
    else {
        Student *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
    student_count++;
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
            free_student(s);
            student_count--;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average() {
    if (student_count == 0) { printf("0\n"); return; }
    int sum = 0;
    Student *s = head;
    while (s) { sum += s->score; s = s->next; }
    printf("%d\n", sum / student_count);
}

void list_all() {
    Student *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *scorestr = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && scorestr) {
                int score = atoi(scorestr);
                if (!note) note = "";
                add_student(name, score, note);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            char *scorestr = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && scorestr) {
                int score = atoi(scorestr);
                if (!note) note = "";
                update_student(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    while (head) {
        Student *tmp = head;
        head = head->next;
        free_student(tmp);
    }
    return 0;
}