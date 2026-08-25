// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student { char *name; int score; char *note; struct Student *next; } Student;
Student *students = NULL;

Student *find_student(const char *name) {
    for (Student *s = students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name); s->score = score; s->note = strdup(note);
    s->next = students; students = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score; free(s->note); s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL;
    for (Student *s = students; s; prev = s, s = s->next) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next; else students = s->next;
            free(s->name); free(s->note); free(s);
            return;
        }
    }
}

void average_score(void) {
    int sum = 0, count = 0;
    for (Student *s = students; s; s = s->next) { sum += s->score; count++; }
    if (count == 0) printf("0\n");
    else printf("%d\n", sum / count);
}

void list_students(void) {
    for (Student *s = students; s; s = s->next)
        printf("%s %d %s\n", s->name, s->score, s->note);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256]; int score; char note[4096];
            if (sscanf(rest, "%255s %d", name, &score) == 2) {
                char *n = rest + strlen(name) + 1;
                while (*n == ' ') n++;
                while (*n >= '0' && *n <= '9') n++;
                while (*n == ' ') n++;
                strcpy(note, n);
                add_student(name, score, note);
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256]; int score; char note[4096];
            if (sscanf(rest, "%255s %d", name, &score) == 2) {
                char *n = rest + strlen(name) + 1;
                while (*n == ' ') n++;
                while (*n >= '0' && *n <= '9') n++;
                while (*n == ' ') n++;
                strcpy(note, n);
                update_student(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    while (students) { Student *s = students; students = s->next; free(s->name); free(s->note); free(s); }
    return 0;
}