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
int student_count = 0;

Student *find_student(const char *name) {
    Student *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (!head) head = s;
    else {
        Student *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = s;
    }
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
    Student *s = head, *prev = NULL;
    while (s && strcmp(s->name, name) != 0) {
        prev = s;
        s = s->next;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else head = s->next;
    free(s->name);
    free(s->note);
    free(s);
    student_count--;
}

void average_score(void) {
    if (student_count == 0) { printf("0\n"); return; }
    int sum = 0;
    Student *s = head;
    while (s) {
        sum += s->score;
        s = s->next;
    }
    printf("%d\n", sum / student_count);
}

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d ", s->name, s->score);
        for (char *c = s->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
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
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[1024];
            int score;
            char *p = line + 4;
            if (sscanf(p, "%255s %d", name, &score) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_student(name, score, p);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256], note[1024];
            int score;
            char *p = line + 7;
            if (sscanf(p, "%255s %d", name, &score) != 2) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            update_student(name, score, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            drop_student(p);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    free(line);
    return 0;
}