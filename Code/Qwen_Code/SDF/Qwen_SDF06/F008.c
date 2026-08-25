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

void add_student(const char *name, int score, const char *note) {
    for (Student *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) return;
    }
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = head;
    head = s;
}

void update_student(const char *name, int score, const char *note) {
    for (Student *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            curr->score = score;
            free(curr->note);
            curr->note = strdup(note);
            return;
        }
    }
}

void drop_student(const char *name) {
    Student *curr = head;
    Student *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Student *next = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void average_scores() {
    int sum = 0;
    int count = 0;
    for (Student *curr = head; curr; curr = curr->next) {
        sum += curr->score;
        count++;
    }
    if (count > 0) printf("%d\n", sum / count);
    else printf("0\n");
}

void list_students() {
    for (Student *curr = head; curr; curr = curr->next) {
        printf("%s %d %s\n", curr->name, curr->score, curr->note);
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
        
        char cmd[16];
        char name[1024], note[4096];
        int score;
        
        int n = sscanf(line, "%15s %1023s %d %[^\n]", cmd, name, &score, note);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0 && n == 4) {
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_scores();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}