// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *data;
} Rec;

int main() {
    Rec *recs = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", recs[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *len_data = p2 + 1;
                char *colon = strchr(len_data, ':');
                if (colon) {
                    *colon = '\0';
                    int length = atoi(len_data);
                    if (length >= 0) {
                        char *data_start = colon + 1;
                        int actual_len = strlen(data_start);
                        int final_len = (length < actual_len) ? length : actual_len;
                        recs = realloc(recs, sizeof(Rec) * (count + 1));
                        recs[count].name = strdup(name);
                        recs[count].data = malloc(final_len + 1);
                        strncpy(recs[count].data, data_start, final_len);
                        recs[count].data[final_len] = '\0';
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recs[i].name, args) == 0) {
                    free(recs[i].name); free(recs[i].data);
                    for (int j = i; j < count - 1; j++) {
                        recs[j] = recs[j + 1];
                    }
                    count--;
                    if (count == 0) { free(recs); recs = NULL; }
                    else { recs = realloc(recs, sizeof(Rec) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recs[i].name, args) == 0) {
                    for (int j = 0; recs[i].data[j]; j++) {
                        if (recs[i].data[j] == ' ') printf("_");
                        else printf("%c", recs[i].data[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recs[i].name); free(recs[i].data);
    }
    free(recs);
    return 0;
}
