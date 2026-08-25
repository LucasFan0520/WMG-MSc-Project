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
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) exit(EXIT_FAILURE);
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (!head) {
        head = s;
    } else {
        Student *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = s;
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) {
        add_student(name, score, note);
        return;
    }
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *curr = head;
    Student *prev = NULL;
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
    int sum = 0;
    int count = 0;
    Student *curr = head;
    while (curr) {
        sum += curr->score;
        count++;
        curr = curr->next;
    }
    if (count > 0) printf("%d\n", sum / count);
    else printf("0\n");
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
        
        char cmd[32] = {0};
        char name[256] = {0};
        int score = 0;
        char note[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %d %[^\n]", cmd, name, &score, note);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 4) {
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0 && matched == 4) {
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}