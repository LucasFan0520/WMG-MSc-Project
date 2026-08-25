// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    int header_len;
    int payload_len;
    char *header;
    char *payload;
} Packet;

Packet *packets = NULL;
int num_packets = 0;

int find_packet(const char *id) {
    for (int i = 0; i < num_packets; i++) {
        if (strcmp(packets[i].id, id) == 0) return i;
    }
    return -1;
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256];
            int hlen, plen;
            sscanf(line, "%*s %s %d %d", id, &hlen, &plen);
            
            char *p = strstr(line, id);
            if (p) {
                p += strlen(id);
                while (*p == ' ') p++;
                p = strchr(p, ' ');
                if (p) {
                    p++;
                    while (*p == ' ') p++;
                    p = strchr(p, ' ');
                    if (p) {
                        p++;
                        while (*p == ' ') p++;
                        p = strchr(p, ' ');
                        if (p) {
                            p++;
                            char *header = p;
                            p = strchr(p, ' ');
                            if (p) {
                                *p = 0;
                                p++;
                                char *payload = p;
                                
                                if ((int)strlen(header) == hlen && (int)strlen(payload) == plen) {
                                    packets = realloc(packets, (num_packets + 1) * sizeof(Packet));
                                    packets[num_packets].id = strdup(id);
                                    packets[num_packets].header_len = hlen;
                                    packets[num_packets].payload_len = plen;
                                    packets[num_packets].header = strdup(header);
                                    packets[num_packets].payload = strdup(payload);
                                    num_packets++;
                                }
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char id[256];
            sscanf(line, "%*s %s", id);
            int idx = find_packet(id);
            if (idx >= 0) {
                free(packets[idx].id);
                free(packets[idx].header);
                free(packets[idx].payload);
                for (int i = idx; i < num_packets - 1; i++) {
                    packets[i] = packets[i + 1];
                }
                num_packets--;
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            char id[256];
            sscanf(line, "%*s %s", id);
            int idx = find_packet(id);
            if (idx >= 0) {
                printf("%s\n", packets[idx].header);
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            char id[256];
            sscanf(line, "%*s %s", id);
            int idx = find_packet(id);
            if (idx >= 0) {
                printf("%s\n", packets[idx].payload);
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long long total = 0;
            for (int i = 0; i < num_packets; i++) {
                total += packets[i].payload_len;
            }
            printf("%lld\n", total);
        }
    }
    
    for (int i = 0; i < num_packets; i++) {
        free(packets[i].id);
        free(packets[i].header);
        free(packets[i].payload);
    }
    free(packets);
    
    return 0;
}