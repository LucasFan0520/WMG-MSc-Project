// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CLASS 1000
#define MAX_STU 1000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char note[MAX_LEN];
} Student;

typedef struct {
    char name[100];
    Student students[MAX_STU];
    int scount;
} Class;

Class classes[MAX_CLASS];
int ccount = 0;

int find_class(const char *name) {
    for (int i = 0; i < ccount; i++) if (strcmp(classes[i].name, name) == 0) return i;
    return -1;
}

int find_student(int cidx, const char *name) {
    for (int i = 0; i < classes[cidx].scount; i++) {
        if (strcmp(classes[cidx].students[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], c1[100], c2[100], sname[100], note[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CLASS") == 0) {
            scanf("%s", c1);
            if (find_class(c1) == -1 && ccount < MAX_CLASS) {
                strcpy(classes[ccount].name, c1);
                classes[ccount].scount = 0;
                ccount++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            scanf("%s %s", c1, sname);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int cidx = find_class(c1);
            if (cidx != -1 && classes[cidx].scount < MAX_STU) {
                strcpy(classes[cidx].students[classes[cidx].scount].name, sname);
                strcpy(classes[cidx].students[classes[cidx].scount].note, note);
                classes[cidx].scount++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            scanf("%s %s %s", c1, c2, sname);
            int i1 = find_class(c1);
            int i2 = find_class(c2);
            if (i1 != -1 && i2 != -1) {
                int sidx = find_student(i1, sname);
                if (sidx != -1 && classes[i2].scount < MAX_STU) {
                    classes[i2].students[classes[i2].scount] = classes[i1].students[sidx];
                    classes[i2].scount++;
                    for (int i = sidx; i < classes[i1].scount - 1; i++) {
                        classes[i1].students[i] = classes[i1].students[i + 1];
                    }
                    classes[i1].scount--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s %s", c1, sname);
            int cidx = find_class(c1);
            if (cidx != -1) {
                int sidx = find_student(cidx, sname);
                if (sidx != -1) {
                    for (int i = sidx; i < classes[cidx].scount - 1; i++) {
                        classes[cidx].students[i] = classes[cidx].students[i + 1];
                    }
                    classes[cidx].scount--;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            scanf("%s", c1);
            int cidx = find_class(c1);
            if (cidx != -1) {
                for (int i = cidx; i < ccount - 1; i++) classes[i] = classes[i + 1];
                ccount--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].scount; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    return 0;
}