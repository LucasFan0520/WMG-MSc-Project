/* F020.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char **body_lines;
    size_t b_count;
    size_t b_capacity;
} Record;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    Record *catalog = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;

    if (fp) {
        int next_is_title = 1;
        Record cur_rec = {NULL, NULL, 0, 0};

        while (getline(&line, &len, fp) != -1) {
            size_t str_len = strlen(line);
            while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
                line[str_len - 1] = '\0';
                str_len--;
            }
            if (strcmp(line, "--") == 0) {
                if (cur_rec.title) {
                    if (count >= capacity) {
                        size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                        Record *temp = realloc(catalog, new_cap * sizeof(Record));
                        if (!temp) break;
                        catalog = temp;
                        capacity = new_cap;
                    }
                    catalog[count] = cur_rec;
                    count++;
                }
                cur_rec.title = NULL;
                cur_rec.body_lines = NULL;
                cur_rec.b_count = 0;
                cur_rec.b_capacity = 0;
                next_is_title = 1;
            } else {
                if (next_is_title) {
                    cur_rec.title = strdup(line);
                    next_is_title = 0;
                } else {
                    if (cur_rec.b_count >= cur_rec.b_capacity) {
                        size_t new_cap = cur_rec.b_capacity == 0 ? 4 : cur_rec.b_capacity * 2;
                        char **temp = realloc(cur_rec.body_lines, new_cap * sizeof(char *));
                        if (!temp) break;
                        cur_rec.body_lines = temp;
                        cur_rec.b_capacity = new_cap;
                    }
                    cur_rec.body_lines[cur_rec.b_count] = strdup(line);
                    if (cur_rec.body_lines[cur_rec.b_count]) {
                        cur_rec.b_count++;
                    }
                }
            }
        }
        if (cur_rec.title) {
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *temp = realloc(catalog, new_cap * sizeof(Record));
                if (temp) {
                    catalog = temp;
                    capacity = new_cap;
                    catalog[count] = cur_rec;
                    count++;
                } else {
                    free(cur_rec.title);
                    for (size_t i = 0; i < cur_rec.b_count; i++) free(cur_rec.body_lines[i]);
                    free(cur_rec.body_lines);
                }
            } else {
                catalog[count] = cur_rec;
                count++;
            }
        }
        fclose(fp);
    }

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *title = p;

        if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    for (size_t j = 0; j < catalog[i].b_count; j++) {
                        printf("%s\n", catalog[i].body_lines[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(catalog[i].title, title) == 0) {
                    free(catalog[i].title);
                    for (size_t j = 0; j < catalog[i].b_count; j++) {
                        free(catalog[i].body_lines[j]);
                    }
                    free(catalog[i].body_lines);
                    for (size_t j = i; j < count - 1; j++) {
                        catalog[j] = catalog[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            Record new_rec;
            new_rec.title = strdup(title);
            new_rec.body_lines = NULL;
            new_rec.b_count = 0;
            new_rec.b_capacity = 0;

            char *sub_line = NULL;
            size_t sub_len = 0;
            while (getline(&sub_line, &sub_len, stdin) != -1) {
                size_t s_len = strlen(sub_line);
                while (s_len > 0 && (sub_line[s_len - 1] == '\n' || sub_line[s_len - 1] == '\r')) {
                    sub_line[s_len - 1] = '\0';
                    s_len--;
                }
                if (strcmp(sub_line, "--") == 0) {
                    break;
                }
                if (new_rec.b_count >= new_rec.b_capacity) {
                    size_t new_cap = new_rec.b_capacity == 0 ? 4 : new_rec.b_capacity * 2;
                    char **temp = realloc(new_rec.body_lines, new_cap * sizeof(char *));
                    if (!temp) break;
                    new_rec.body_lines = temp;
                    new_rec.b_capacity = new_cap;
                }
                new_rec.body_lines[new_rec.b_count] = strdup(sub_line);
                if (new_rec.body_lines[new_rec.b_count]) {
                    new_rec.b_count++;
                }
            }
            free(sub_line);

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *temp = realloc(catalog, new_cap * sizeof(Record));
                if (!temp) {
                    free(new_rec.title);
                    for (size_t i = 0; i < new_rec.b_count; i++) free(new_rec.body_lines[i]);
                    free(new_rec.body_lines);
                    continue;
                }
                catalog = temp;
                capacity = new_cap;
            }
            catalog[count] = new_rec;
            count++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", catalog[i].title);
                for (size_t j = 0; j < catalog[i].b_count; j++) {
                    printf("%s\n", catalog[i].body_lines[j]);
                }
                printf("--\n");
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(catalog[i].title);
        for (size_t j = 0; j < catalog[i].b_count; j++) {
            free(catalog[i].body_lines[j]);
        }
        free(catalog[i].body_lines);
    }
    free(catalog);
    return 0;
}
