// F040.c
#define _GNU_SOURCE
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

int main(void) {
    Packet *cache = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id = p;
            char *space1 = strchr(id, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *hl_str = space1 + 1;
            while (*hl_str == ' ') hl_str++;
            char *space2 = strchr(hl_str, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *pl_str = space2 + 1;
            while (*pl_str == ' ') pl_str++;
            char *space3 = strchr(pl_str, ' ');
            if (!space3) continue;
            *space3 = '\0';
            char *header = space3 + 1;
            while (*header == ' ') header++;
            char *space4 = strchr(header, ' ');
            char *payload = "";
            if (space4) {
                *space4 = '\0';
                payload = space4 + 1;
                while (*payload == ' ') payload++;
            }
            int h_len = atoi(hl_str);
            int p_len = atoi(pl_str);
            if (h_len < 0 || p_len < 0) continue;
            if ((int)strlen(header) < h_len || (int)strlen(payload) < p_len) continue;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Packet *tmp = realloc(cache, new_cap * sizeof(Packet));
                if (!tmp) return 1;
                cache = tmp;
                capacity = new_cap;
            }
            cache[count].id = strdup(id);
            cache[count].h_len = h_len;
            cache[count].p_len = p_len;
            char *h_buf = malloc(h_len + 1);
            char *p_buf = malloc(p_len + 1);
            if (!h_buf || !p_buf) return 1;
            strncpy(h_buf, header, h_len);
            h_buf[h_len] = '\0';
            strncpy(p_buf, payload, p_len);
            p_buf[p_len] = '\0';
            cache[count].header = h_buf;
            cache[count].payload = p_buf;
            count++;
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            while (*id == ' ') id++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(cache[found].id);
                free(cache[found].header);
                free(cache[found].payload);
                for (int i = found; i < count - 1; i++) {
                    cache[i] = cache[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            while (*id == ' ') id++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", cache[found].header);
            else printf("NOT_FOUND\n");
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            while (*id == ' ') id++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", cache[found].payload);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < count; i++) {
                total += cache[i].p_len;
            }
            printf("%lld\n", total);
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(cache[i].id);
        free(cache[i].header);
        free(cache[i].payload);
    }
    free(cache);
    return 0;
}
