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
static int scount = 0;
static int scap = 0;

static int ensure_cap(void) {
    if (scount >= scap) {
        int newcap = scap == 0 ? 8 : scap * 2;
        Student *tmp = realloc(students, sizeof(Student) * newcap);
        if (!tmp) return -1;
        students = tmp;
        scap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_student(const char *name) {
    for (int i = 0; i < scount; i++) {
        if (strcmp(students[i].name, name) == 0)
            return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char name[256] = {0};
            int score = 0;
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            if (sscanf(line + offset + off2, " %d%n", &score, &off3) != 1) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            if (find_student(name) >= 0) continue;
            if (ensure_cap() != 0) continue;
            students[scount].name = dup_str(name);
            students[scount].score = score;
            students[scount].note = dup_str(note);
            if (!students[scount].name || !students[scount].note) {
                free(students[scount].name);
                free(students[scount].note);
                continue;
            }
            scount++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[256] = {0};
            int score = 0;
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, " %255s%n", name, &off2) != 1) continue;
            if (sscanf(line + offset + off2, " %d%n", &score, &off3) != 1) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            int idx = find_student(name);
            if (idx < 0) continue;
            char *nn = dup_str(note);
            if (!nn) continue;
            free(students[idx].note);
            students[idx].note = nn;
            students[idx].score = score;
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, " %255s", name) != 1) continue;
            int idx = find_student(name);
            if (idx < 0) continue;
            free(students[idx].name);
            free(students[idx].note);
            for (int i = idx; i < scount - 1; i++)
                students[i] = students[i + 1];
            scount--;
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (scount == 0) {
                printf("0\n");
            } else {
                long sum = 0;
                for (int i = 0; i < scount; i++)
                    sum += students[i].score;
                printf("%d\n", (int)(sum / scount));
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
    }
    for (int i = 0; i < scount; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
    return 0;
}