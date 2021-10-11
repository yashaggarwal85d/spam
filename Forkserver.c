#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

void func(int sockfd, short cport)
{
    char buff[256];
    int n;

    while (1)
    {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From client(%d): %sTo client(%d) : ", cport, buff, cport);

        bzero(buff, sizeof(buff));
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));

        //sendto
        // if msg contains "Exit" then server exit and chat ended.
        // if (strncmp("exit", buff, 4) == 0)
        // {
        //     printf("Server Exit...\n");
        //     break;
        // }
    }
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    pid_t pid;
    // int sockfd = socket(domain, type, protocol)
    // domain: integer, communication domain e.g., AF_INET (IPv4protocol) , AF_INET6 (IPv6 protocol)
    // type: communication type
    // SOCK_STREAM: TCP(reliable, connection oriented)
    // SOCK_DGRAM: UDP(unreliable, connectionless)
    // protocol: Protocol value for Internet Protocol(IP), which is 0.
    // This is the same number which appears on protocol field in the
    // IP header of a packet.(man protocols for more details)
    // This helps in manipulating options for the socket referred by
    // the file descriptor sockfd. This is completely optional, but it
    // helps in reuse of address and port. Prevents error such as:
    // “address already in use”.

    if (server_fd == -1)
    {
        printf("error creating server");
        exit(0);
    }
    else
        printf("server creation successful\n");

    // constructing new struct - serverAddress of type sockaddr_in
    struct sockaddr_in server_address, client_address;

    // assinging details to struct
    server_address.sin_family = AF_INET;         //domain
    server_address.sin_addr.s_addr = INADDR_ANY; //address
    server_address.sin_port = htons(9001);       //port

    // int bind(int sockfd, const struct sockaddr *addr, socklen_taddrlen);
    // After creation of the socket, bind function binds the socket to
    // the address and port number specified in addr(custom data
    // structure). In the example code, we bind the server to the
    // localhost, hence we use INADDR_ANY to specify the IP address.

    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("binding failed");
        exit(0);
    }

    // int listen(int sockfd, int backlog);
    // It puts the server socket in a passive mode, where it waits for
    // the client to approach the server to make a connection. The
    // backlog, defines the maximum length to which the queue of
    // pending connections for sockfd may grow. If a connection
    // request arrives when the queue is full, the client may receive
    // an error with an indication of ECONNREFUSED.

    if (listen(server_fd, 5) < 0)
    {
        printf("listen failed");
        exit(0);
    }

    // int new_socket= accept(int sockfd, struct sockaddr *addr,socklen_t *addrlen);
    // It extracts the first connection request on the queue of
    // pending connections for the listening socket, sockfd, creates a
    // new connected socket, and returns a new file descriptor
    // referring to that socket. At this point, connection is established
    // between client and server, and they are ready to transfer data.
    while (1)
    {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0)
        {
            printf("server acccept failed...\n");
            exit(0);
        }
        else
        {
            printf("server acccept the client...\n");
            short c_port = ntohs(client_address.sin_port);

            pid = fork();

            if (pid == 0)
            {
                close(server_fd);
                func(client_fd, c_port);
            }
        }
    }

    close(server_fd);
}