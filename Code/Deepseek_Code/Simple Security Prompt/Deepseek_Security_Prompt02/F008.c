/* F008.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *students = NULL;
int student_count = 0;

Student *find_student(const char *name) {
    Student *s = students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_student(Student *s) {
    free(s->name);
    free(s->note);
    free(s);
}

void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
        return;
    }
    s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = students;
    students = s;
    student_count++;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *curr = students;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else students = curr->next;
            free_student(curr);
            student_count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void average_score(void) {
    if (student_count == 0) {
        printf("0\n");
        return;
    }
    int sum = 0;
    Student *s = students;
    while (s) {
        sum += s->score;
        s = s->next;
    }
    printf("%d\n", sum / student_count);
}

void list_students(void) {
    Student *s = students;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    int score;
    char note[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &score) != 2) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            p += (score >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            add_student(name, score, p);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &score) != 2) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            p += (score >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            update_student(name, score, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }

    while (students) {
        Student *tmp = students;
        students = students->next;
        free_student(tmp);
    }
    return 0;
}