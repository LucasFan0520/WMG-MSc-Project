// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *roster = NULL;

Student *find_student(const char *name) {
    Student *cur = roster;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = safe_strdup(name);
    s->score = score;
    s->note = safe_strdup(note);
    s->next = roster;
    roster = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = safe_strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *cur = roster;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else roster = cur->next;
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
    int total = 0, count = 0;
    Student *cur = roster;
    while (cur) {
        total += cur->score;
        count++;
        cur = cur->next;
    }
    if (count == 0) printf("0\n");
    else printf("%d\n", total / count);
}

void list_students(void) {
    Student *cur = roster;
    while (cur) {
        printf("%s %d %s\n", cur->name, cur->score, cur->note);
        cur = cur->next;
    }
}

void free_all(void) {
    Student *cur = roster;
    while (cur) {
        Student *next = cur->next;
        free(cur->name);
        free(cur->note);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            if (n < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            int score = 0;
            char note[MAX_LINE] = "";
            if (sscanf(p, "%d %[^\n]", &score, note) >= 1) {
                if (strcmp(cmd, "ADD") == 0) add_student(arg1, score, note);
                else update_student(arg1, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0 && n >= 2) {
            drop_student(arg1);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }
    free_all();
    return 0;
}