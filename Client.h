#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <string.h> /* memset */
// pthreads
#include <pthread.h>
#include <sys/types.h>

#include <string>
#define String std::string
#include <list>

#define STDIN 0

#include "Term.h"

void *client_loop_wrapper(void * client);

class Client {
    public:

        /* ports, sockets, etc */
        short int port_num;
        int server_sock;
        struct sockaddr_in server;
        fd_set read_fds, master;
        int fdmax;
        String destination_ip;

        /* data */
        char buffer[65536];
        std::list <String> buffers;

        /* locks */
        pthread_mutex_t buffer_lock;

        /* threads */
        pthread_t * thread;
        pthread_attr_t pthread_custom_attr; // for custom settings

        /* set up socket */
        void open_socket(int port_number) {
            port_num = port_number;
            server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (server_sock < 0) {perror("socket() failed");abort();}

        }

        /* initialization - client */
        void initialize_client(String ip) {
            memset(&server, 0, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.s_addr = inet_addr(ip.c_str());
            //server.sin_addr.s_addr = inet_addr("127.0.0.1");
            //            server.sin_addr.s_addr = inet_addr("66.154.118.218");
            server.sin_port = htons(port_num);		
        }

        /* connect to the socket */
        void connect_to_server() {
            socklen_t addrlen = sizeof(server);
            int connection = connect(server_sock, (const struct sockaddr *) &server, addrlen );
            if (connection < 0) {perror("can't connect");abort();}		
        }


        /* init sets and add listener to the master list of socket descriptors */
        void init_fds() {
            fdmax = server_sock;
            FD_ZERO(&read_fds);
            FD_ZERO(&master);
            FD_SET(server_sock, &master);
            FD_SET(STDIN, &master);
        }

        void read_from_server(int fd) {
            /* receiving data */
            memset(buffer, '\0', sizeof(buffer));
            int expected_data_len = sizeof(buffer);
            int read_bytes = recv(fd, buffer, expected_data_len, 0);
            if (read_bytes == 0) { 
                printf("connection closed: exiting ... (0_o)\n");abort();
            } else if (read_bytes < 0) {
                perror("recv failed");abort();
            } else {
                buffer[read_bytes] = '\0';
                printf("incoming: %s\n",buffer);
            }
        }	

        void read_from_stdin() {
            /* send data */
            int rd = read(STDIN, buffer, sizeof(buffer));
            int sent_bytes = send(server_sock, buffer, rd-1, 0);
            if (sent_bytes < 0) perror("send failed");
        }

        void send_msg(String msg) {
            int sent_bytes = send(server_sock, msg.c_str(), msg.size(), 0);
            if (sent_bytes < 0) perror("send failed");
        }

        void main_loop() {

            struct timeval tv;

            while (true) {
                read_fds = master;
                /* set up time out for select */
                tv.tv_sec = 5;  /* timers must go here! */
                tv.tv_usec = 500000; 

                int sel = select(fdmax+1, &read_fds, NULL, NULL, &tv);
                if (sel == -1) {
                    perror("select()");abort();
                } else if (sel) {

                    /* data must be ready - iterate over all descriptors */
                    for (int i = 0; i <= fdmax; i++) {
                        if (FD_ISSET(i, &read_fds)) {
                            if (i == server_sock) {
                                pthread_mutex_lock(&buffer_lock);
                                read_from_server(server_sock);
                                pthread_mutex_unlock(&buffer_lock);
                            } else if (i == STDIN) {
                                pthread_mutex_lock(&buffer_lock);
                                read_from_stdin();
                                pthread_mutex_unlock(&buffer_lock);
                            }
                        } /* end if ISSET */
                    } /* end of fs_set iterator */

                } else {
                    //printf("client: no data within %d seconds\n", tv.tv_sec);
                }

                flush_buffer();

            } // end while loop

            close(server_sock);
            close(STDIN);

        }

        void start_client(String ip, int port_number) {
            open_socket(port_number);
            initialize_client(ip);
            connect_to_server();
            init_fds();

            /* init mutex */
            pthread_mutex_init(&buffer_lock, NULL);

            /* init thread */
            thread = (pthread_t *) malloc(sizeof(pthread_t));
            pthread_attr_init(&pthread_custom_attr);            
            pthread_create(thread, &pthread_custom_attr, client_loop_wrapper, this);

        }

        void flush_buffer() {
                pthread_mutex_lock(&buffer_lock);
                String retval = String();
                for(std::list<String>::iterator it=buffers.begin(); it != buffers.end(); it++) 
                    Term::parse(*it, false);
                buffers.clear();
                pthread_mutex_unlock(&buffer_lock);
        }


        void disconnect(int port) {
            if (thread) free(thread);
        }
};

