/* F033.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *name;
    char *department;
    char *title;
} Employee;

static Employee *emps = NULL;
static int ecount = 0;
static int ecap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_emp(const char *id) {
    for (int i = 0; i < ecount; i++)
        if (strcmp(emps[i].id, id) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char id[256] = {0}, name[256] = {0}, dept[256] = {0}, title[256] = {0};
            if (sscanf(line + offset, "%255s %255s %255s %255s", id, name, dept, title) != 4) continue;
            if (find_emp(id) >= 0) continue;
            if (ecount >= ecap) {
                ecap = ecap == 0 ? 8 : ecap * 2;
                emps = realloc(emps, sizeof(Employee) * ecap);
            }
            emps[ecount].id = dup_str(id);
            emps[ecount].name = dup_str(name);
            emps[ecount].department = dup_str(dept);
            emps[ecount].title = dup_str(title);
            ecount++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256] = {0}, name[256] = {0}, dept[256] = {0}, title[256] = {0};
            if (sscanf(line + offset, "%255s %255s %255s %255s", id, name, dept, title) != 4) continue;
            int idx = find_emp(id);
            if (idx < 0) continue;
            free(emps[idx].name);
            free(emps[idx].department);
            free(emps[idx].title);
            emps[idx].name = dup_str(name);
            emps[idx].department = dup_str(dept);
            emps[idx].title = dup_str(title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, "%255s", id) != 1) continue;
            int idx = find_emp(id);
            if (idx < 0) continue;
            free(emps[idx].id);
            free(emps[idx].name);
            free(emps[idx].department);
            free(emps[idx].title);
            for (int i = idx; i < ecount - 1; i++) emps[i] = emps[i + 1];
            ecount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256] = {0};
            if (sscanf(line + offset, "%255s", id) != 1) continue;
            int idx = find_emp(id);
            if (idx >= 0) printf("%s %s %s %s\n", emps[idx].id, emps[idx].name, emps[idx].department, emps[idx].title);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < ecount; i++) {
                printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].department, emps[i].title);
            }
        }
    }
    for (int i = 0; i < ecount; i++) {
        free(emps[i].id);
        free(emps[i].name);
        free(emps[i].department);
        free(emps[i].title);
    }
    free(emps);
    return 0;
}