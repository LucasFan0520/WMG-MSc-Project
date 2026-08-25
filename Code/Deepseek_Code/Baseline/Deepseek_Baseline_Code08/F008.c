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

Student* find_student(const char *name) {
    Student *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
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
        Student *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int average_score() {
    if (!head) return 0;
    int sum = 0, count = 0;
    Student *cur = head;
    while (cur) {
        sum += cur->score;
        count++;
        cur = cur->next;
    }
    return count ? sum / count : 0;
}

void free_all() {
    Student *cur = head;
    while (cur) {
        Student *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *score_str = strtok(NULL, " ");
            if (!score_str) continue;
            int score = atoi(score_str);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *score_str = strtok(NULL, " ");
            if (!score_str) continue;
            int score = atoi(score_str);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            update_student(name, score, note);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            printf("%d\n", average_score());
        } else if (strcmp(cmd, "LIST") == 0) {
            Student *cur = head;
            while (cur) {
                printf("%s %d %s\n", cur->name, cur->score, cur->note);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}