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
    Student *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = head;
    head = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *cur = head, *prev = NULL;
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

void average_score(void) {
    int sum = 0, count = 0;
    Student *cur = head;
    while (cur) {
        sum += cur->score;
        count++;
        cur = cur->next;
    }
    if (count == 0) { printf("0\n"); return; }
    printf("%d\n", sum / count);
}

void list_students(void) {
    Student *cur = head;
    while (cur) {
        printf("%s %d %s\n", cur->name, cur->score, cur->note);
        cur = cur->next;
    }
}

void cleanup_students(void) {
    Student *cur = head;
    while (cur) {
        Student *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[2048];
        int score;
        int n = sscanf(line, "%31s %255s %d %2047s", cmd, name, &score, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            // skip score
            char *p = note;
            while (*p && *p != ' ') p++;
            while (*p && *p == ' ') p++;
            add_student(name, score, p);
        } else if (strcmp(cmd, "UPDATE") == 0 && n >= 4) {
            char *note = line + strlen(cmd) + 1 + strlen(name) + 1;
            char *p = note;
            while (*p && *p != ' ') p++;
            while (*p && *p == ' ') p++;
            update_student(name, score, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    cleanup_students();
    return 0;
}