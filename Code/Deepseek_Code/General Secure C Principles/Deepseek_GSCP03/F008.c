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
Student *tail = NULL;
int count = 0;

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
    if (!s) return;
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    count++;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->note);
            free(curr);
            count--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void average_score(void) {
    if (count == 0) {
        printf("0\n");
        return;
    }
    int sum = 0;
    Student *s = head;
    while (s) {
        sum += s->score;
        s = s->next;
    }
    printf("%d\n", sum / count);
}

void list_students(void) {
    Student *s = head;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Student *s = head;
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int score;
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &score) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    add_student(name, score, note);
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d", name, &score) == 2) {
                char *note = p;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                while (*note && *note != ' ') note++;
                while (*note && *note == ' ') note++;
                if (*note) {
                    update_student(name, score, note);
                }
            }
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
    return 0;
}