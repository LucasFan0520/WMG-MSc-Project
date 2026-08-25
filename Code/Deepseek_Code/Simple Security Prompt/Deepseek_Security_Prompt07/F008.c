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

Student *students = NULL;

Student *find_student(const char *name) {
    Student *s = students;
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
        s->note = strdup(note);
    } else {
        s = malloc(sizeof(Student));
        s->name = strdup(name);
        s->score = score;
        s->note = strdup(note);
        s->next = students;
        students = s;
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
    Student *prev = NULL;
    Student *s = students;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else students = s->next;
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
    Student *s = students;
    int sum = 0, cnt = 0;
    while (s) {
        sum += s->score;
        cnt++;
        s = s->next;
    }
    printf("%d\n", cnt ? sum / cnt : 0);
}

void list_students() {
    Student *s = students;
    while (s) {
        printf("%s:%d:", s->name, s->score);
        for (char *p = s->note; *p; p++) putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
        s = s->next;
    }
}

void free_all() {
    Student *s = students;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int score;
            char note[4096];
            if (sscanf(rest, "%255s %d %[^\n]", name, &score, note) == 3) {
                add_student(name, score, note);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score;
            char note[4096];
            if (sscanf(rest, "%255s %d %[^\n]", name, &score, note) == 3) {
                update_student(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                drop_student(name);
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}