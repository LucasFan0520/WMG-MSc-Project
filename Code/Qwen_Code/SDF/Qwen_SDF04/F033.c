// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* id; char* name; char* dept; char* title; } Employee;

Employee* emps = NULL;
size_t e_count = 0, e_cap = 0;

void cleanup() {
    for (size_t i = 0; i < e_count; i++) {
        free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
    }
    free(emps);
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
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* dept = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;

            if (e_count == e_cap) {
                size_t new_cap = e_cap == 0 ? 4 : e_cap * 2;
                Employee* temp = realloc(emps, new_cap * sizeof(Employee));
                if (!temp) { cleanup(); free(line); return 1; }
                emps = temp; e_cap = new_cap;
            }
            emps[e_count].id = strdup(id);
            emps[e_count].name = strdup(name);
            emps[e_count].dept = strdup(dept);
            emps[e_count].title = strdup(title);
            if (!emps[e_count].id || !emps[e_count].name || !emps[e_count].dept || !emps[e_count].title) {
                cleanup(); free(line); return 1;
            }
            e_count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* dept = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;

            for (size_t i = 0; i < e_count; i++) {
                if (strcmp(emps[i].id, id) == 0) {
                    char *n = strdup(name), *d = strdup(dept), *t = strdup(title);
                    if (!n || !d || !t) { free(n); free(d); free(t); cleanup(); free(line); return 1; }
                    free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                    emps[i].name = n; emps[i].dept = d; emps[i].title = t;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < e_count; i++) {
                if (strcmp(emps[i].id, ptr) == 0) {
                    free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                    for (size_t j = i; j < e_count - 1; j++) emps[j] = emps[j + 1];
                    e_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < e_count; i++) {
                if (strcmp(emps[i].id, ptr) == 0) {
                    printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < e_count; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}