// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stu {
    char *name, *note;
    int score;
    struct Stu *next;
} Stu;

Stu *head = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], name[256], note[1024];
    int score;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %d", name, &score); read_rest(note);
            Stu *s = head;
            while (s && strcmp(s->name, name) != 0) s = s->next;
            if (s) { s->score = score; free(s->note); s->note = strdup(note); }
            else {
                Stu *ns = malloc(sizeof(Stu));
                ns->name = strdup(name); ns->score = score; ns->note = strdup(note); ns->next = head; head = ns;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", name);
            Stu *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->note); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0, cnt = 0;
            for (Stu *s = head; s; s = s->next) { sum += s->score; cnt++; }
            printf("%d\n", cnt > 0 ? sum / cnt : 0);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Stu *s = head; s; s = s->next) printf("%s %d %s\n", s->name, s->score, s->note);
        }
    }
    return 0;
}