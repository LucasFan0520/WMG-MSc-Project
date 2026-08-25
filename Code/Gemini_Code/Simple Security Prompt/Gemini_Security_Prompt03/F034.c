// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Note34;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Note34 *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            FILE *fp = fopen(argv[1], "r");
            if (fp) {
                Note34 *temp_list = NULL;
                int temp_count = 0;
                int valid = 1;
                while (1) {
                    char *fline = read_line(fp);
                    if (!fline) break;
                    char *fp_ptr = fline;
                    while (*fp_ptr == ' ') fp_ptr++;
                    if (*fp_ptr == '\0') {
                        free(fline);
                        continue;
                    }
                    char *name = fp_ptr;
                    while (*fp_ptr && *fp_ptr != ' ') fp_ptr++;
                    if (*fp_ptr) { *fp_ptr = '\0'; fp_ptr++; }
                    while (*fp_ptr == ' ') fp_ptr++;
                    char *note = fp_ptr;
                    if (strlen(name) == 0 || strlen(note) == 0) {
                        valid = 0;
                        free(fline);
                        break;
                    }
                    temp_list = realloc(temp_list, (temp_count + 1) * sizeof(Note34));
                    temp_list[temp_count].name = strdup(name);
                    temp_list[temp_count].note = strdup(note);
                    temp_count++;
                    free(fline);
                }
                fclose(fp);
                if (valid) {
                    for (int i = 0; i < count; i++) {
                        if (list[i].name) {
                            free(list[i].name);
                            free(list[i].note);
                        }
                    }
                    free(list);
                    list = temp_list;
                    count = temp_count;
                } else {
                    for (int i = 0; i < temp_count; i++) {
                        free(temp_list[i].name);
                        free(temp_list[i].note);
                    }
                    free(temp_list);
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            list = realloc(list, (count + 1) * sizeof(Note34));
            list[count].name = strdup(name);
            list[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    list[i].name = NULL;
                    list[i].note = NULL;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s %s\n", list[i].name, list[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].note);
        }
    }
    free(list);
    return 0;
}
