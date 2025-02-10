#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_HOPS 30
#define TIMEOUT_SEC 1

using namespace std;

unsigned short checksum(void *b, int len) {
    unsigned short *buf = (unsigned short *)b;
    unsigned int sum = 0;
    unsigned short result;
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void traceroute(const char *target) {
    int sockfd;
    struct sockaddr_in dest_addr;
    struct hostent *host;

    if ((host = gethostbyname(target)) == NULL) {
        cerr << "Error: Unable to resolve hostname" << endl;
        return;
    }
    
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    memcpy(&dest_addr.sin_addr, host->h_addr, host->h_length);

    cout << "Traceroute to " << target << " (" << inet_ntoa(dest_addr.sin_addr) << "), " << MAX_HOPS << " hops max" << endl;

    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Socket error");
        return;
    }

    struct sockaddr_in recv_addr;
    socklen_t recv_addr_len = sizeof(recv_addr);
    char recv_buffer[512];

    for (int ttl = 1; ttl <= MAX_HOPS; ttl++) {
        if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
            perror("setsockopt failed");
            return;
        }

        struct icmp icmp_packet;
        memset(&icmp_packet, 0, sizeof(icmp_packet));
        icmp_packet.icmp_type = ICMP_ECHO;
        icmp_packet.icmp_code = 0;
        icmp_packet.icmp_id = getpid();
        icmp_packet.icmp_seq = ttl;
        icmp_packet.icmp_cksum = checksum(&icmp_packet, sizeof(icmp_packet));

        struct timeval start_time, end_time;
        gettimeofday(&start_time, NULL);

        if (sendto(sockfd, &icmp_packet, sizeof(icmp_packet), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) <= 0) {
            cout << ttl << ": Error sending packet" << endl;
            continue;
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);
        struct timeval timeout = {TIMEOUT_SEC, 0};

        if (select(sockfd + 1, &fds, NULL, NULL, &timeout) > 0) {
            if (recvfrom(sockfd, recv_buffer, sizeof(recv_buffer), 0, (struct sockaddr *)&recv_addr, &recv_addr_len) > 0) {
                gettimeofday(&end_time, NULL);
                cout << ttl << ": " << inet_ntoa(recv_addr.sin_addr) << "  " << ((end_time.tv_usec - start_time.tv_usec) / 1000.0) << " ms" << endl;
                if (recv_addr.sin_addr.s_addr == dest_addr.sin_addr.s_addr) {
                    cout << "Reached destination." << endl;
                    break;
                }
            }
        } else {
            cout << ttl << ": * Request timed out" << endl;
        }
    }
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <hostname or IP>" << endl;
        return 1;
    }
    traceroute(argv[1]);
    return 0;
}