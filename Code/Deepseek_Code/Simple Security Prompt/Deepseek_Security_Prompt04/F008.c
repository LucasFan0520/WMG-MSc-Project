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

Student *find_student(char *name) {
    for (Student *s = head; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

void add_student(char *name, int score, char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    } else {
        Student *new = malloc(sizeof(Student));
        new->name = strdup(name);
        new->score = score;
        new->note = strdup(note);
        new->next = head;
        head = new;
    }
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
    for (Student *s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else head = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
    }
}

void average_score(void) {
    int total = 0, count = 0;
    for (Student *s = head; s; s = s->next) {
        total += s->score;
        count++;
    }
    printf("%d\n", count ? total / count : 0);
}

void list_all(void) {
    for (Student *s = head; s; s = s->next)
        printf("%s %d %s\n", s->name, s->score, s->note);
}

void free_all(void) {
    Student *s = head;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_student(arg1, atoi(arg2), arg3);
        } else if (strcmp(cmd, "UPDATE") == 0 && n == 4) {
            update_student(arg1, atoi(arg2), arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n == 2) {
            drop_student(arg1);
        } else if (strcmp(cmd, "AVERAGE") == 0 && n == 1) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}