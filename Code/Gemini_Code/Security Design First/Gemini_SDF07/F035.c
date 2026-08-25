/* F035.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *args = line + 8;
            char *space = strchr(args, ' ');
            char *name = NULL;
            char *pay = NULL;
            if (space) {
                *space = '\0';
                name = args;
                pay = space + 1;
            } else {
                name = args;
                pay = "";
            }
            char *n_name = strdup(name);
            char *n_pay = strdup(pay);
            if (n_name && n_pay) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Job *tmp = realloc(queue, new_cap * sizeof(Job));
                    if (tmp) {
                        queue = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_pay);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                queue[count].name = n_name;
                queue[count].payload = n_pay;
                count++;
            } else {
                free(n_name); free(n_pay);
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *oldN = args;
                char *newN = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldN) == 0) {
                        char *n_name = strdup(newN);
                        char *n_pay = strdup(queue[i].payload);
                        if (n_name && n_pay) {
                            if (count >= capacity) {
                                int new_cap = capacity == 0 ? 4 : capacity * 2;
                                Job *tmp = realloc(queue, new_cap * sizeof(Job));
                                if (tmp) {
                                    queue = tmp;
                                    capacity = new_cap;
                                } else {
                                    free(n_name); free(n_pay);
                                    break;
                                }
                            }
                            queue[count].name = n_name;
                            queue[count].payload = n_pay;
                            count++;
                        } else {
                            free(n_name); free(n_pay);
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
