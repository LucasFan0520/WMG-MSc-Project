// F008.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main(void) {
    Student *gb = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *sc_str = space1 + 1;
            while (*sc_str == ' ') sc_str++;
            char *space2 = strchr(sc_str, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int score = atoi(sc_str);
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Student *tmp = realloc(gb, new_cap * sizeof(Student));
                if (!tmp) {
                    free(line);
                    for(int i=0; i<count; i++) { free(gb[i].name); free(gb[i].note); }
                    free(gb);
                    return 1;
                }
                gb = tmp;
                capacity = new_cap;
            }
            char *nn = strdup(name);
            char *nt = strdup(note);
            if (!nn || !nt) {
                free(nn); free(nt); free(line);
                for(int i=0; i<count; i++) { free(gb[i].name); free(gb[i].note); }
                free(gb);
                return 1;
            }
            gb[count].name = nn;
            gb[count].score = score;
            gb[count].note = nt;
            count++;
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *sc_str = space1 + 1;
            while (*sc_str == ' ') sc_str++;
            char *space2 = strchr(sc_str, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            int score = atoi(sc_str);
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(gb[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nt = strdup(note);
                if (!nt) {
                    free(line);
                    for(int i=0; i<count; i++) { free(gb[i].name); free(gb[i].note); }
                    free(gb);
                    return 1;
                }
                free(gb[found].note);
                gb[found].score = score;
                gb[found].note = nt;
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(gb[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(gb[found].name);
                free(gb[found].note);
                for (int i = found; i < count - 1; i++) {
                    gb[i] = gb[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += gb[i].score;
                }
                printf("%lld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", gb[i].name, gb[i].score, gb[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(gb[i].name);
        free(gb[i].note);
    }
    free(gb);
    return 0;
}
