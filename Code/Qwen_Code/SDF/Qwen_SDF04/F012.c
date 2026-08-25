// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    char* note;
} Person;

Person* queue = NULL;
size_t q_count = 0;
size_t q_cap = 0;

void cleanup() {
    for (size_t i = 0; i < q_count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
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
        
        if (strcmp(cmd, "JOIN") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            if (q_count == q_cap) {
                size_t new_cap = q_cap == 0 ? 4 : q_cap * 2;
                Person* temp = realloc(queue, new_cap * sizeof(Person));
                if (!temp) { cleanup(); free(line); return 1; }
                queue = temp;
                q_cap = new_cap;
            }
            queue[q_count].name = strdup(name);
            queue[q_count].note = strdup(note);
            if (!queue[q_count].name || !queue[q_count].note) { cleanup(); free(line); return 1; }
            q_count++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (q_count > 0) {
                printf("%s\n", queue[0].name);
                free(queue[0].name);
                free(queue[0].note);
                for (size_t i = 0; i < q_count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                q_count--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < q_count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (size_t j = i; j < q_count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    q_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REJOIN") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            if (q_count == q_cap) {
                size_t new_cap = q_cap == 0 ? 4 : q_cap * 2;
                Person* temp = realloc(queue, new_cap * sizeof(Person));
                if (!temp) { cleanup(); free(line); return 1; }
                queue = temp;
                q_cap = new_cap;
            }
            queue[q_count].name = strdup(name);
            queue[q_count].note = strdup(note);
            if (!queue[q_count].name || !queue[q_count].note) { cleanup(); free(line); return 1; }
            q_count++;
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < q_count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (size_t i = 0; i < q_count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}