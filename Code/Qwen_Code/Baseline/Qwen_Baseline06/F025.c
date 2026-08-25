// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Stu;
typedef struct { char *name; Stu *stus; int n_stu; } Class;
Class *classes; int n_class;

int find_class(char *name) {
    for(int i=0; i<n_class; i++) if(!strcmp(classes[i].name, name)) return i;
    return -1;
}

int find_stu(Class *c, char *name) {
    for(int i=0; i<c->n_stu; i++) if(!strcmp(c->stus[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], c1[100], c2[100], name[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "CLASS")) {
            scanf("%s", c1);
            classes = realloc(classes, (n_class+1)*sizeof(Class));
            classes[n_class].name = strdup(c1);
            classes[n_class].stus = NULL;
            classes[n_class].n_stu = 0;
            n_class++;
        } else if(!strcmp(cmd, "STUDENT")) {
            scanf("%s %s", c1, name);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = find_class(c1);
            if(i != -1) {
                classes[i].stus = realloc(classes[i].stus, (classes[i].n_stu+1)*sizeof(Stu));
                classes[i].stus[classes[i].n_stu].name = strdup(name);
                classes[i].stus[classes[i].n_stu].note = strdup(note);
                classes[i].n_stu++;
            }
        } else if(!strcmp(cmd, "TRANSFER")) {
            scanf("%s %s %s", c1, c2, name);
            int i1 = find_class(c1), i2 = find_class(c2);
            if(i1 != -1 && i2 != -1) {
                int s = find_stu(&classes[i1], name);
                if(s != -1) {
                    classes[i2].stus = realloc(classes[i2].stus, (classes[i2].n_stu+1)*sizeof(Stu));
                    classes[i2].stus[classes[i2].n_stu] = classes[i1].stus[s];
                    classes[i2].n_stu++;
                    for(int k=s; k<classes[i1].n_stu-1; k++) classes[i1].stus[k] = classes[i1].stus[k+1];
                    classes[i1].n_stu--;
                }
            }
        } else if(!strcmp(cmd, "DROP")) {
            scanf("%s %s", c1, name);
            int i = find_class(c1);
            if(i != -1) {
                int s = find_stu(&classes[i], name);
                if(s != -1) {
                    free(classes[i].stus[s].name);
                    free(classes[i].stus[s].note);
                    for(int k=s; k<classes[i].n_stu-1; k++) classes[i].stus[k] = classes[i].stus[k+1];
                    classes[i].n_stu--;
                }
            }
        } else if(!strcmp(cmd, "DELETECLASS")) {
            scanf("%s", c1);
            int i = find_class(c1);
            if(i != -1) {
                free(classes[i].name);
                for(int j=0; j<classes[i].n_stu; j++) {
                    free(classes[i].stus[j].name);
                    free(classes[i].stus[j].note);
                }
                free(classes[i].stus);
                for(int k=i; k<n_class-1; k++) classes[k] = classes[k+1];
                n_class--;
            }
        } else if(!strcmp(cmd, "PRINT")) {
            for(int i=0; i<n_class; i++) {
                printf("%s\n", classes[i].name);
                for(int j=0; j<classes[i].n_stu; j++) {
                    printf("%s %s\n", classes[i].stus[j].name, classes[i].stus[j].note);
                }
            }
        }
    }
    return 0;
}