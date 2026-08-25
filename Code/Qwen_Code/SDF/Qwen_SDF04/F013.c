// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* title;
    char* note;
} Task;

Task* todo = NULL;
size_t t_count = 0;
size_t t_cap = 0;

Task* doing = NULL;
size_t d_count = 0;
size_t d_cap = 0;

Task* done = NULL;
size_t dn_count = 0;
size_t dn_cap = 0;

void cleanup() {
    for (size_t i = 0; i < t_count; i++) { free(todo[i].title); free(todo[i].note); }
    free(todo);
    for (size_t i = 0; i < d_count; i++) { free(doing[i].title); free(doing[i].note); }
    free(doing);
    for (size_t i = 0; i < dn_count; i++) { free(done[i].title); free(done[i].note); }
    free(done);
}

void add_to_list(Task** list, size_t* count, size_t* cap, const char* title, const char* note) {
    if (*count == *cap) {
        size_t new_cap = *cap == 0 ? 4 : *cap * 2;
        Task* temp = realloc(*list, new_cap * sizeof(Task));
        if (!temp) exit(1);
        *list = temp;
        *cap = new_cap;
    }
    (*list)[*count].title = strdup(title);
    (*list)[*count].note = strdup(note);
    if (!(*list)[*count].title || !(*list)[*count].note) exit(1);
    (*count)++;
}

int remove_from_list(Task* list, size_t* count, const char* title) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp(list[i].title, title) == 0) {
            free(list[i].title);
            free(list[i].note);
            for (size_t j = i; j < *count - 1; j++) {
                list[j] = list[j + 1];
            }
            (*count)--;
            return 1;
        }
    }
    return 0;
}

int main() {
    char* line = NULL;
    size_t len = 0;
    
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "TODO") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            add_to_list(&todo, &t_count, &t_cap, title, note);
        } else if (strcmp(cmd, "START") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(todo[i].title, title) == 0) {
                    add_to_list(&doing, &d_count, &d_cap, todo[i].title, todo[i].note);
                    free(todo[i].title);
                    free(todo[i].note);
                    for (size_t j = i; j < t_count - 1; j++) {
                        todo[j] = todo[j + 1];
                    }
                    t_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < d_count; i++) {
                if (strcmp(doing[i].title, title) == 0) {
                    add_to_list(&done, &dn_count, &dn_cap, doing[i].title, doing[i].note);
                    free(doing[i].title);
                    free(doing[i].note);
                    for (size_t j = i; j < d_count - 1; j++) {
                        doing[j] = doing[j + 1];
                    }
                    d_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            if (!remove_from_list(todo, &t_count, title)) {
                if (!remove_from_list(doing, &d_count, title)) {
                    remove_from_list(done, &dn_count, title);
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(todo[i].title, title) == 0) {
                    printf("todo %s\n", todo[i].note);
                    return 0;
                }
            }
            for (size_t i = 0; i < d_count; i++) {
                if (strcmp(doing[i].title, title) == 0) {
                    printf("doing %s\n", doing[i].note);
                    return 0;
                }
            }
            for (size_t i = 0; i < dn_count; i++) {
                if (strcmp(done[i].title, title) == 0) {
                    printf("done %s\n", done[i].note);
                    return 0;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:\n");
            for (size_t i = 0; i < t_count; i++) printf("%s %s\n", todo[i].title, todo[i].note);
            printf("doing:\n");
            for (size_t i = 0; i < d_count; i++) printf("%s %s\n", doing[i].title, doing[i].note);
            printf("done:\n");
            for (size_t i = 0; i < dn_count; i++) printf("%s %s\n", done[i].title, done[i].note);
        }
    }
    
    free(line);
    cleanup();
    return 0;
}