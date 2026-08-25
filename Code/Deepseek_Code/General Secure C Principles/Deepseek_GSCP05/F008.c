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
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = students;
    students = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *cur = students;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else students = cur->next;
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
    Student *s = students;
    while (s) {
        sum += s->score;
        count++;
        s = s->next;
    }
    if (count == 0) printf("0\n");
    else printf("%d\n", sum / count);
}

void list_students(void) {
    Student *s = students;
    while (s) {
        printf("%s %d: ", s->name, s->score);
        char *n = s->note;
        for (; *n; n++) putchar(*n == ' ' ? '_' : *n);
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    Student *s = students;
    while (s) {
        Student *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            int score;
            char *score_start = space + 1;
            while (*score_start == ' ') score_start++;
            if (sscanf(score_start, "%d", &score) != 1) continue;
            char *note_start = strchr(score_start, ' ');
            if (!note_start) continue;
            while (*note_start == ' ') note_start++;
            add_student(name, score, note_start);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            int score;
            char *score_start = space + 1;
            while (*score_start == ' ') score_start++;
            if (sscanf(score_start, "%d", &score) != 1) continue;
            char *note_start = strchr(score_start, ' ');
            if (!note_start) continue;
            while (*note_start == ' ') note_start++;
            update_student(name, score, note_start);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}