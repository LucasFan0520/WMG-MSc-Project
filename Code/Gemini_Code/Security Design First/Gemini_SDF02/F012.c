/* F012.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

int main(void) {
    Person *queue = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

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

        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Person *temp = realloc(queue, new_cap * sizeof(Person));
                if (!temp) break;
                queue = temp;
                capacity = new_cap;
            }
            queue[count].name = strdup(name_str);
            queue[count].note = strdup(note_str);
            if (queue[count].name && queue[count].note) {
                count++;
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (count > 0) {
                printf("%s\n", queue[0].name);
                free(queue[0].name);
                free(queue[0].note);
                for (size_t j = 0; j < count - 1; j++) {
                    queue[j] = queue[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name_str = p;
            for (size_t i = 0; i < count; ) {
                if (strcmp(queue[i].name, name_str) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name_str) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
