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

Student *head = NULL, *tail = NULL;

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
        s->note = malloc(strlen(note) + 1);
        strcpy(s->note, note);
    } else {
        Student *new = malloc(sizeof(Student));
        new->name = malloc(strlen(name) + 1);
        strcpy(new->name, name);
        new->score = score;
        new->note = malloc(strlen(note) + 1);
        strcpy(new->note, note);
        new->next = NULL;
        if (tail) {
            tail->next = new;
            tail = new;
        } else {
            head = tail = new;
        }
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void average_score(void) {
    int total = 0, count = 0;
    Student *s = head;
    while (s) {
        total += s->score;
        count++;
        s = s->next;
    }
    printf("%d\n", count ? total / count : 0);
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *score = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score && note) add_student(name, atoi(score), note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            char *score = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score && note) update_student(name, atoi(score), note);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free(line);
    free_all();
    return 0;
}