// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *time;
    char *label;
    char *note;
} Activity;

int main() {
    Activity *log = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char time_str[256], label_str[256];
            int t_len = 0, l_len = 0;
            if (sscanf(ptr, "%255s%n", time_str, &t_len) > 0) {
                char *ptr2 = ptr + t_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", label_str, &l_len) > 0) {
                    char *note_str = ptr2 + l_len;
                    while (*note_str == ' ') note_str++;
                    log = realloc(log, (count + 1) * sizeof(Activity));
                    log[count].time = strdup(time_str);
                    log[count].label = strdup(label_str);
                    log[count].note = strdup(note_str);
                    count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char time_str[256], label_str[256];
            if (sscanf(ptr, "%255s %255s", time_str, label_str) == 2) {
                int i = 0;
                while (i < count) {
                    if (strcmp(log[i].time, time_str) == 0 && strcmp(log[i].label, label_str) == 0) {
                        free(log[i].time);
                        free(log[i].label);
                        free(log[i].note);
                        for (int j = i; j < count - 1; j++) {
                            log[j] = log[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count > 0) {
                    log = realloc(log, count * sizeof(Activity));
                } else {
                    free(log);
                    log = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char label_str[256];
            if (sscanf(ptr, "%255s", label_str) > 0) {
                int earliest_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(log[i].label, label_str) == 0) {
                        if (earliest_idx == -1 || strcmp(log[i].time, log[earliest_idx].time) < 0) {
                            earliest_idx = i;
                        }
                    }
                }
                if (earliest_idx != -1) {
                    printf("%s %s %s\n", log[earliest_idx].time, log[earliest_idx].label, log[earliest_idx].note);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            for (int i = 0; i < count - 1; i++) {
                for (int j = 0; j < count - i - 1; j++) {
                    int cmp = strcmp(log[j].time, log[j + 1].time);
                    if (cmp == 0) {
                        cmp = strcmp(log[j].label, log[j + 1].label);
                    }
                    if (cmp > 0) {
                        Activity temp = log[j];
                        log[j] = log[j + 1];
                        log[j + 1] = temp;
                    }
                }
            }
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", log[i].time, log[i].label, log[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (int i = 0; i < count; i++) {
                free(log[i].time);
                free(log[i].label);
                free(log[i].note);
            }
            free(log);
            log = NULL;
            count = 0;
        }
    }
    for (int i = 0; i < count; i++) {
        free(log[i].time);
        free(log[i].label);
        free(log[i].note);
    }
    free(log);
    return 0;
}
