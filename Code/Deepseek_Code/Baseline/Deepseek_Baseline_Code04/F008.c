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
        Student *ns = malloc(sizeof(Student));
        ns->name = strdup(name);
        ns->score = score;
        ns->note = strdup(note);
        ns->next = students;
        students = ns;
    }
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    }
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

int average_score(void) {
    int sum = 0, count = 0;
    Student *s = students;
    while (s) {
        sum += s->score;
        count++;
        s = s->next;
    }
    return count ? sum / count : 0;
}

void free_students(void) {
    while (students) {
        Student *tmp = students;
        students = students->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256];
        int score;
        int n = sscanf(line, "%s %s %d", cmd, name, &score);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            // skip score digits
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            if (*p) add_student(name, score, p);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            if (*p) update_student(name, score, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (n < 2) continue;
            drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            printf("%d\n", average_score());
        } else if (strcmp(cmd, "LIST") == 0) {
            Student *s = students;
            while (s) {
                printf("%s\n", s->name);
                s = s->next;
            }
        }
    }
    free_students();
    return 0;
}