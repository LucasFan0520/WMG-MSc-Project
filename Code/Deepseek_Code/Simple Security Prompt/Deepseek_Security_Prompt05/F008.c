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

Student *roster = NULL;

Student *find_student(const char *name) {
    Student *s = roster;
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
        return;
    }
    s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = roster;
    roster = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *s = roster, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else roster = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    if (!roster) {
        printf("0\n");
        return;
    }
    int sum = 0, count = 0;
    Student *s = roster;
    while (s) {
        sum += s->score;
        count++;
        s = s->next;
    }
    printf("%d\n", sum / count);
}

void list_students(void) {
    Student *s = roster;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (n < 3) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            if (strcmp(cmd, "ADD") == 0)
                add_student(arg1, atoi(arg2), note);
            else
                update_student(arg1, atoi(arg2), note);
        } else if (strcmp(cmd, "DROP") == 0) {
            drop_student(arg1);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    while (roster) {
        Student *tmp = roster;
        roster = roster->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    return 0;
}