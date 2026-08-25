// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 100
#define MAX_NOTE 1000

typedef struct {
    char title[MAX_TITLE];
    char note[MAX_NOTE];
} Task;

Task todo[1000], doing[1000], done[1000];
int todo_count = 0, doing_count = 0, done_count = 0;

int find_in_list(Task *list, int count, const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(list[i].title, title) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], title[MAX_TITLE], note[MAX_NOTE];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "TODO") == 0) {
            scanf("%s", title);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            strcpy(todo[todo_count].title, title);
            strcpy(todo[todo_count].note, note);
            todo_count++;
        } else if (strcmp(cmd, "START") == 0) {
            scanf("%s", title);
            int idx = find_in_list(todo, todo_count, title);
            if (idx != -1) {
                strcpy(doing[doing_count].title, todo[idx].title);
                strcpy(doing[doing_count].note, todo[idx].note);
                doing_count++;
                
                for (int i = idx; i < todo_count - 1; i++) {
                    todo[i] = todo[i + 1];
                }
                todo_count--;
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            scanf("%s", title);
            int idx = find_in_list(doing, doing_count, title);
            if (idx != -1) {
                strcpy(done[done_count].title, doing[idx].title);
                strcpy(done[done_count].note, doing[idx].note);
                done_count++;
                
                for (int i = idx; i < doing_count - 1; i++) {
                    doing[i] = doing[i + 1];
                }
                doing_count--;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s", title);
            int idx;
            
            idx = find_in_list(todo, todo_count, title);
            if (idx != -1) {
                for (int i = idx; i < todo_count - 1; i++) {
                    todo[i] = todo[i + 1];
                }
                todo_count--;
                continue;
            }
            
            idx = find_in_list(doing, doing_count, title);
            if (idx != -1) {
                for (int i = idx; i < doing_count - 1; i++) {
                    doing[i] = doing[i + 1];
                }
                doing_count--;
                continue;
            }
            
            idx = find_in_list(done, done_count, title);
            if (idx != -1) {
                for (int i = idx; i < done_count - 1; i++) {
                    done[i] = done[i + 1];
                }
                done_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            int idx;
            
            idx = find_in_list(todo, todo_count, title);
            if (idx != -1) {
                printf("todo %s\n", todo[idx].note);
                continue;
            }
            
            idx = find_in_list(doing, doing_count, title);
            if (idx != -1) {
                printf("doing %s\n", doing[idx].note);
                continue;
            }
            
            idx = find_in_list(done, done_count, title);
            if (idx != -1) {
                printf("done %s\n", done[idx].note);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:\n");
            for (int i = 0; i < todo_count; i++) {
                printf("  %s %s\n", todo[i].title, todo[i].note);
            }
            printf("doing:\n");
            for (int i = 0; i < doing_count; i++) {
                printf("  %s %s\n", doing[i].title, doing[i].note);
            }
            printf("done:\n");
            for (int i = 0; i < done_count; i++) {
                printf("  %s %s\n", done[i].title, done[i].note);
            }
        }
    }
    
    return 0;
}