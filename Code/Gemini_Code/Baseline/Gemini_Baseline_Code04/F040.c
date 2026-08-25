// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int h_len;
    int p_len;
    char *header;
    char *payload;
} Packet;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Packet *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            char *id = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *hl_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    char *pl_s = p;
                    while (*p && *p != ' ') p++;
                    if (*p == ' ') {
                        *p = '\0';
                        p++;
                        char *hdr = p;
                        while (*p && *p != ' ') p++;
                        if (*p == ' ') {
                            *p = '\0';
                            char *pay = p + 1;
                            arr = realloc(arr, (count + 1) * sizeof(Packet));
                            arr[count].id = strdup(id);
                            arr[count].h_len = atoi(hl_s);
                            arr[count].p_len = atoi(pl_s);
                            arr[count].header = strdup(hdr);
                            arr[count].payload = strdup(pay);
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].header);
                    free(arr[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s\n", arr[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long total = 0;
            for (int i = 0; i < count; i++) {
                total += arr[i].p_len;
            }
            printf("%ld\n", total);
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].header);
        free(arr[i].payload);
    }
    free(arr);
    return 0;
}
