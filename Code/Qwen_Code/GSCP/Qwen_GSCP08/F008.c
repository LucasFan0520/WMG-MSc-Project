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

Student *find_student(char *name) {
    for (Student *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

void add_student(char *name, int score, char *note) {
    Student *s = find_student(name);
    if (!s) {
        s = malloc(sizeof(Student));
        s->name = strdup(name);
        s->next = head;
        head = s;
    } else {
        free(s->note);
    }
    s->score = score;
    s->note = strdup(note);
}

void update_student(char *name, int score, char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    }
}

void drop_student(char *name) {
    Student *prev = NULL;
    Student *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_average() {
    int sum = 0, count = 0;
    for (Student *s = head; s; s = s->next) {
        sum += s->score;
        count++;
    }
    printf("%d\n", count > 0 ? sum / count : 0);
}

void list_students() {
    for (Student *s = head; s; s = s->next) {
        printf("%s %d %s\n", s->name, s->score, s->note);
    }
}

int main() {
    char cmd[20], name[100], note[1000];
    int score;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            scanf("%99s%d", name, &score);
            fgets(note, sizeof(note), stdin);
            while (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            if (strcmp(cmd, "ADD") == 0) add_student(name, score, note);
            else update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%99s", name);
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            print_average();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    return 0;
}