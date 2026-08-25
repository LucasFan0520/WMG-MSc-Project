// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stu {
    char *name, *note;
    struct Stu *next;
} Stu;

typedef struct Cls {
    char *name;
    Stu *students;
    struct Cls *next;
} Cls;

Cls *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

Cls *find_cls(const char *name) {
    for (Cls *c = head; c; c = c->next) if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Stu *find_stu(Cls *c, const char *name) {
    for (Stu *s = c->students; s; s = s->next) if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

int main() {
    char cmd[20], c1[256], c2[256], name[256], note[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CLASS") == 0) {
            scanf("%s", c1);
            Cls *c = malloc(sizeof(Cls));
            c->name = strdup(c1); c->students = NULL; c->next = head; head = c;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            scanf("%s %s", c1, name); read_rest(note, sizeof(note));
            Cls *c = find_cls(c1);
            if (c) {
                Stu *s = malloc(sizeof(Stu));
                s->name = strdup(name); s->note = strdup(note); s->next = c->students; c->students = s;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            scanf("%s %s %s", c1, c2, name);
            Cls *src = find_cls(c1), *dst = find_cls(c2);
            if (src && dst) {
                Stu *prev = NULL, *curr = src->students;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else src->students = curr->next;
                        curr->next = dst->students; dst->students = curr; break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s %s", c1, name);
            Cls *c = find_cls(c1);
            if (c) {
                Stu *prev = NULL, *curr = c->students;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else c->students = curr->next;
                        free(curr->name); free(curr->note); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            scanf("%s", c1);
            Cls *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, c1) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->students) { Stu *ns = curr->students->next; free(curr->students->name); free(curr->students->note); free(curr->students); curr->students = ns; }
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Cls *c = head; c; c = c->next) {
                printf("%s:\n", c->name);
                for (Stu *s = c->students; s; s = s->next) printf("  %s %s\n", s->name, s->note);
            }
        }
    }
    return 0;
}