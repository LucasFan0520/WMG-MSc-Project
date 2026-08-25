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
Student *tail = NULL;

Student *find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = malloc(strlen(note)+1);
        strcpy(s->note, note);
    } else {
        Student *new = malloc(sizeof(Student));
        new->name = malloc(strlen(name)+1);
        strcpy(new->name, name);
        new->score = score;
        new->note = malloc(strlen(note)+1);
        strcpy(new->note, note);
        new->next = NULL;
        if (tail) tail->next = new;
        else head = new;
        tail = new;
    }
}

void update_student(const char *name, int score, const char *note) {
    add_student(name, score, note); // same logic
}

void drop_student(const char *name) {
    Student *prev = NULL;
    Student *s = head;
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

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
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

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *score_str = strtok(NULL, " ");
            if (!score_str) continue;
            int score = atoi(score_str);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *score_str = strtok(NULL, " ");
            if (!score_str) continue;
            int score = atoi(score_str);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(rest, " ");
            if (name) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}