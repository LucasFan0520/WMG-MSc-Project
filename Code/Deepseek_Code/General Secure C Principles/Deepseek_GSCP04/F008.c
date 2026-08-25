/* F008.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

static Student *students = NULL;
static size_t scount = 0, scap = 0;

static Student *find_student(const char *name) {
    for (size_t i = 0; i < scount; i++) {
        if (strcmp(students[i].name, name) == 0)
            return &students[i];
    }
    return NULL;
}

static void add_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (s) {
        s->score = score;
        free(s->note);
        s->note = strdup(note);
    } else {
        if (scount == scap) {
            scap = scap ? scap * 2 : 4;
            students = realloc(students, scap * sizeof(Student));
            if (!students) { perror("realloc"); exit(1); }
        }
        students[scount].name = strdup(name);
        students[scount].score = score;
        students[scount].note = strdup(note);
        scount++;
    }
}

static void drop_student(const char *name) {
    for (size_t i = 0; i < scount; i++) {
        if (strcmp(students[i].name, name) == 0) {
            free(students[i].name);
            free(students[i].note);
            for (size_t j = i; j < scount - 1; j++)
                students[j] = students[j + 1];
            scount--;
            return;
        }
    }
}

static void average_score(void) {
    if (scount == 0) {
        printf("0\n");
        return;
    }
    long sum = 0;
    for (size_t i = 0; i < scount; i++) sum += students[i].score;
    printf("%ld\n", sum / (long)scount);
}

static void list_students(void) {
    for (size_t i = 0; i < scount; i++) {
        printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *scstr = strtok(NULL, " ");
            if (!name || !scstr) continue;
            int score = atoi(scstr);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            char *scstr = strtok(NULL, " ");
            if (!name || !scstr) continue;
            int score = atoi(scstr);
            char *note = strtok(NULL, "");
            if (!note) note = "";
            Student *s = find_student(name);
            if (s) {
                s->score = score;
                free(s->note);
                s->note = strdup(note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    for (size_t i = 0; i < scount; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
    return 0;
}