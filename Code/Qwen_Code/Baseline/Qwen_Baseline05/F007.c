// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LABEL 100
#define MAX_NOTE 1000

typedef struct {
    int time;
    char label[MAX_LABEL];
    char note[MAX_NOTE];
} Activity;

Activity activities[10000];
int act_count = 0;

int main() {
    char cmd[20], label[MAX_LABEL], note[MAX_NOTE];
    int time;
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%d %s", &time, label);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            strcpy(activities[act_count].label, label);
            strcpy(activities[act_count].note, note);
            activities[act_count].time = time;
            act_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d %s", &time, label);
            int i = 0;
            while (i < act_count) {
                if (activities[i].time == time && strcmp(activities[i].label, label) == 0) {
                    for (int j = i; j < act_count - 1; j++) {
                        activities[j] = activities[j + 1];
                    }
                    act_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", label);
            int earliest = -1;
            for (int i = 0; i < act_count; i++) {
                if (strcmp(activities[i].label, label) == 0) {
                    if (earliest == -1 || activities[i].time < activities[earliest].time) {
                        earliest = i;
                    }
                }
            }
            if (earliest != -1) {
                printf("%d %s %s\n", activities[earliest].time, activities[earliest].label, activities[earliest].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            for (int i = 0; i < act_count - 1; i++) {
                for (int j = 0; j < act_count - i - 1; j++) {
                    int swap = 0;
                    if (activities[j].time > activities[j + 1].time) {
                        swap = 1;
                    } else if (activities[j].time == activities[j + 1].time && 
                               strcmp(activities[j].label, activities[j + 1].label) > 0) {
                        swap = 1;
                    }
                    if (swap) {
                        Activity temp = activities[j];
                        activities[j] = activities[j + 1];
                        activities[j + 1] = temp;
                    }
                }
            }
            for (int i = 0; i < act_count; i++) {
                printf("%d %s %s\n", activities[i].time, activities[i].label, activities[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            act_count = 0;
        }
    }
    
    return 0;
}