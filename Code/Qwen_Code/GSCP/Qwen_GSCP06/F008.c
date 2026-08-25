// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Stu {
    char* name;
    int score;
    char* note;
    struct Stu* next;
} Stu;

Stu* head = NULL;

Stu* find_stu(const char* name) {
    for (Stu* s = head; s; s = s->next) {
        if (strcmp(s->name, name) == 0) return s;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* name = get_word(&p);
            char* sc_str = get_word(&p);
            Stu* n = malloc(sizeof(Stu));
            n->name = strdup(name);
            n->score = atoi(sc_str);
            n->note = strdup(p);
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char* name = get_word(&p);
            char* sc_str = get_word(&p);
            Stu* s = find_stu(name);
            if (s) {
                s->score = atoi(sc_str);
                free(s->note);
                s->note = strdup(p);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char* name = get_word(&p);
            Stu* prev = NULL;
            for (Stu* s = head; s; prev = s, s = s->next) {
                if (strcmp(s->name, name) == 0) {
                    if (prev) prev->next = s->next;
                    else head = s->next;
                    free(s->name);
                    free(s->note);
                    free(s);
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0, count = 0;
            for (Stu* s = head; s; s = s->next) {
                sum += s->score;
                count++;
            }
            if (count > 0) printf("%d\n", sum / count);
            else printf("0\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Stu* s = head; s; s = s->next) {
                printf("%s %d %s\n", s->name, s->score, s->note);
            }
        }
    }
    return 0;
}