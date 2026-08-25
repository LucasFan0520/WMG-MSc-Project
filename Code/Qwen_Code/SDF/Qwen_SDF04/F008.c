// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    int score;
    char* note;
} Student;

Student* students = NULL;
size_t s_count = 0;
size_t s_cap = 0;

void cleanup() {
    for (size_t i = 0; i < s_count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
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
        
        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* score_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            if (s_count == s_cap) {
                size_t new_cap = s_cap == 0 ? 4 : s_cap * 2;
                Student* temp = realloc(students, new_cap * sizeof(Student));
                if (!temp) { cleanup(); free(line); return 1; }
                students = temp;
                s_cap = new_cap;
            }
            students[s_count].name = strdup(name);
            students[s_count].score = atoi(score_str);
            students[s_count].note = strdup(note);
            if (!students[s_count].name || !students[s_count].note) { cleanup(); free(line); return 1; }
            s_count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* score_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    students[i].score = atoi(score_str);
                    char* temp = strdup(note);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(students[i].note);
                    students[i].note = temp;
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    free(students[i].name);
                    free(students[i].note);
                    for (size_t j = i; j < s_count - 1; j++) {
                        students[j] = students[j + 1];
                    }
                    s_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (s_count == 0) {
                printf("0\n");
            } else {
                long sum = 0;
                for (size_t i = 0; i < s_count; i++) {
                    sum += students[i].score;
                }
                printf("%ld\n", sum / (long)s_count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < s_count; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}