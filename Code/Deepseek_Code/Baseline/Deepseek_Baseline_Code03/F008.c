// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    char *name;
    int score;
    char *note;
    struct student *next;
};

struct student *students = NULL;
struct student *tail = NULL;

struct student *find_student(char *name) {
    struct student *s = students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(char *name, int score, char *note) {
    struct student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = malloc(strlen(note) + 1);
        strcpy(s->note, note);
    } else {
        s = malloc(sizeof(struct student));
        s->name = malloc(strlen(name) + 1);
        strcpy(s->name, name);
        s->score = score;
        s->note = malloc(strlen(note) + 1);
        strcpy(s->note, note);
        s->next = NULL;
        if (tail) {
            tail->next = s;
            tail = s;
        } else {
            students = tail = s;
        }
    }
}

void update_student(char *name, int score, char *note) {
    struct student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = malloc(strlen(note) + 1);
        strcpy(s->note, note);
    }
}

void drop_student(char *name) {
    struct student *s = students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else students = s->next;
            if (tail == s) tail = prev;
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
    int sum = 0, count = 0;
    struct student *s = students;
    while (s) {
        sum += s->score;
        count++;
        s = s->next;
    }
    if (count == 0) printf("0\n");
    else printf("%d\n", sum / count);
}

void list_students() {
    struct student *s = students;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256];
        int score;
        char note[4096];
        int n = sscanf(line, "%31s %255s %d %4095[^\n]", cmd, name, &score, note);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 4) {
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    return 0;
}