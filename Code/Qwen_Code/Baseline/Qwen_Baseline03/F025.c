// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char note[1000];
} Stu;

typedef struct {
    char name[100];
    Stu stus[1000];
    int cnt;
} Cls;

Cls clss[1000];
int cn = 0;

int findCls(char *name) {
    for (int i = 0; i < cn; i++) {
        if (!strcmp(clss[i].name, name)) return i;
    }
    return -1;
}

int findStu(Cls *c, char *name) {
    for (int i = 0; i < c->cnt; i++) {
        if (!strcmp(c->stus[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], stu[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "CLASS")) {
            scanf("%s", name);
            strcpy(clss[cn].name, name);
            clss[cn].cnt = 0;
            cn++;
        } else if (!strcmp(cmd, "STUDENT")) {
            scanf("%s %s", name, stu);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = findCls(name);
            if (i != -1) {
                strcpy(clss[i].stus[clss[i].cnt].name, stu);
                strcpy(clss[i].stus[clss[i].cnt].note, note);
                clss[i].cnt++;
            }
        } else if (!strcmp(cmd, "TRANSFER")) {
            char from[100], to[100];
            scanf("%s %s %s", from, to, stu);
            int i1 = findCls(from);
            int i2 = findCls(to);
            if (i1 != -1 && i2 != -1) {
                int j = findStu(&clss[i1], stu);
                if (j != -1) {
                    clss[i2].stus[clss[i2].cnt] = clss[i1].stus[j];
                    clss[i2].cnt++;
                    for (int k = j; k < clss[i1].cnt - 1; k++) {
                        clss[i1].stus[k] = clss[i1].stus[k + 1];
                    }
                    clss[i1].cnt--;
                }
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s %s", name, stu);
            int i = findCls(name);
            if (i != -1) {
                int j = findStu(&clss[i], stu);
                if (j != -1) {
                    for (int k = j; k < clss[i].cnt - 1; k++) {
                        clss[i].stus[k] = clss[i].stus[k + 1];
                    }
                    clss[i].cnt--;
                }
            }
        } else if (!strcmp(cmd, "DELETECLASS")) {
            scanf("%s", name);
            int i = findCls(name);
            if (i != -1) {
                for (int j = i; j < cn - 1; j++) {
                    clss[j] = clss[j + 1];
                }
                cn--;
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < cn; i++) {
                printf("%s\n", clss[i].name);
                for (int j = 0; j < clss[i].cnt; j++) {
                    printf("%s %s\n", clss[i].stus[j].name, clss[i].stus[j].note);
                }
            }
        }
    }
    return 0;
}