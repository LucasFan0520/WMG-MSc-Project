/* F040.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *header;
    char *payload;
    int payload_len;
} PacketRecord;

int main(void) {
    PacketRecord *cache = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "PACKET ", 7) == 0) {
            char *args = line + 7;
            char *p1 = strchr(args, ' ');
            if (!p1) continue;
            *p1 = '\0';
            char *id = args;
            char *rest1 = p1 + 1;
            char *p2 = strchr(rest1, ' ');
            if (!p2) { *p1 = ' '; continue; }
            *p2 = '\0';
            int hlen = atoi(rest1);
            char *rest2 = p2 + 1;
            char *p3 = strchr(rest2, ' ');
            if (!p3) { *p1 = ' '; *p2 = ' '; continue; }
            *p3 = '\0';
            int plen = atoi(rest2);
            char *rest3 = p3 + 1;
            char *p4 = strchr(rest3, ' ');
            char *hdr = NULL;
            char *pay = NULL;
            if (p4) {
                *p4 = '\0';
                hdr = rest3;
                pay = p4 + 1;
            } else {
                hdr = rest3;
                pay = "";
            }
            if ((int)strlen(hdr) == hlen && (int)strlen(pay) == plen) {
                char *n_id = strdup(id);
                char *n_hdr = strdup(hdr);
                char *n_pay = strdup(pay);
                if (n_id && n_hdr && n_pay) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        PacketRecord *tmp = realloc(cache, new_cap * sizeof(PacketRecord));
                        if (tmp) {
                            cache = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_id); free(n_hdr); free(n_pay);
                            *p1 = ' '; *p2 = ' '; *p3 = ' '; if (p4) *p4 = ' ';
                            continue;
                        }
                    }
                    cache[count].id = n_id;
                    cache[count].header = n_hdr;
                    cache[count].payload = n_pay;
                    cache[count].payload_len = plen;
                    count++;
                } else {
                    free(n_id); free(n_hdr); free(n_pay);
                }
            }
            *p1 = ' '; *p2 = ' '; *p3 = ' '; if (p4) *p4 = ' ';
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *id = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    free(cache[i].id);
                    free(cache[i].header);
                    free(cache[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        cache[j] = cache[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "HEADER ", 7) == 0) {
            char *id = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].header);
                    break;
                }
            }
        } else if (strncmp(line, "PAYLOAD ", 8) == 0) {
            char *id = line + 8;
            for (int i = 0; i < count; i++) {
                if (strcmp(cache[i].id, id) == 0) {
                    printf("%s\n", cache[i].payload);
                    break;
                }
            }
        } else if (strcmp(line, "STATS") == 0) {
            long long total_bytes = 0;
            for (int i = 0; i < count; i++) {
                total_bytes += cache[i].payload_len;
            }
            printf("%lld\n", total_bytes);
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
