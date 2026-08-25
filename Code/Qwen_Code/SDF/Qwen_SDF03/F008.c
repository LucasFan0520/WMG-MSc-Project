// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; int score; } Stu;
Stu *stus = NULL;
size_t count = 0, cap = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

size_t find_stu(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(stus[i].name, name) == 0) return i;
    return count;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *name = get_token(&ptr);
            char *sc_str = get_token(&ptr);
            if (name && sc_str) {
                int sc = atoi(sc_str);
                size_t idx = find_stu(name);
                if (idx == count) {
                    if (count == cap) {
                        size_t new_cap = cap ? cap * 2 : 4;
                        Stu *tmp = realloc(stus, new_cap * sizeof(Stu));
                        if (!tmp) continue;
                        stus = tmp; cap = new_cap;
                    }
                    stus[count].name = strdup(name);
                    idx = count++;
                } else {
                    free(stus[idx].note);
                }
                stus[idx].score = sc;
                stus[idx].note = strdup(ptr);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                size_t idx = find_stu(name);
                if (idx < count) {
                    free(stus[idx].name); free(stus[idx].note);
                    stus[idx] = stus[--count];
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) printf("0\n");
            else {
                long sum = 0;
                for (size_t i = 0; i < count; i++) sum += stus[i].score;
                printf("%ld\n", sum / (long)count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
    }
    for (size_t i = 0; i < count; i++) { free(stus[i].name); free(stus[i].note); }
    free(stus);
    return 0;
}