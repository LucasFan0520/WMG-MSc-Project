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

Student *find_student(const char *name) {
    Student *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    } else {
        s = malloc(sizeof(Student));
        s->name = strdup(name);
        s->score = score;
        s->note = strdup(note);
        s->next = head;
        head = s;
    }
}

void drop_student(const char *name) {
    Student *curr = head, *prev = NULL;
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

void average_score() {
    int sum = 0, count = 0;
    Student *curr = head;
    while (curr) { sum += curr->score; count++; curr = curr->next; }
    if (count > 0) printf("%d\n", sum / count);
}

void list_students() {
    Student *curr = head;
    while (curr) {
        printf("%s %d %s\n", curr->name, curr->score, curr->note);
        curr = curr->next;
    }
}

void free_all() {
    Student *curr = head;
    while (curr) {
        Student *next = curr->next;
        free(curr->name);
        free(curr->note);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], name[256], note[4096] = {0};
        int score;
        int n = sscanf(line, "%31s %255s %d %4095[^\n]", cmd, name, &score, note);
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) add_student(name, score, note);
        else if (strcmp(cmd, "UPDATE") == 0 && n >= 3) add_student(name, score, note);
        else if (strcmp(cmd, "DROP") == 0 && n >= 2) drop_student(name);
        else if (strcmp(cmd, "AVERAGE") == 0) average_score();
        else if (strcmp(cmd, "LIST") == 0) list_students();
    }
    free_all();
    return 0;
}