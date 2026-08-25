/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL;
int line_count = 0;
int capacity = 0;

void ensure_capacity(int needed) {
    if (needed > capacity) {
        int newcap = capacity ? capacity * 2 : 8;
        while (newcap < needed) newcap *= 2;
        lines = realloc(lines, sizeof(char*) * newcap);
        capacity = newcap;
    }
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    ensure_capacity(line_count + 1);
    for (int i = line_count; i > index; i--) lines[i] = lines[i-1];
    lines[index] = strdup(text);
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    free(lines[index]);
    for (int i = index; i < line_count - 1; i++) lines[i] = lines[i+1];
    line_count--;
}

void patch_line(int index, const char *text) {
    if (index < 0 || index >= line_count) return;
    free(lines[index]);
    lines[index] = strdup(text);
}

void print_lines(void) {
    for (int i = 0; i < line_count; i++) {
        char *out = strdup(lines[i]);
        for (char *c = out; *c; c++) if (*c == ' ') *c = '_';
        printf("%s\n", out);
        free(out);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx = atoi(arg1);
            char *txt = line + strlen(cmd) + strlen(arg1) + 2;
            while (*txt == ' ') txt++;
            insert_line(idx, txt);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *txt = line + strlen(cmd) + 1;
            while (*txt == ' ') txt++;
            append_line(txt);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_line(atoi(arg1));
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx = atoi(arg1);
            char *txt = line + strlen(cmd) + strlen(arg1) + 2;
            while (*txt == ' ') txt++;
            patch_line(idx, txt);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    for (int i = 0; i < line_count; i++) free(lines[i]);
    free(lines);
    return 0;
}