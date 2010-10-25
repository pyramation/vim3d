#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h> /* select */
#include <unistd.h>  /* close */
#include <arpa/inet.h>
#include <string.h>
// pthreads
#include <sys/types.h>
#include <pthread.h>

#include <string>
#define String std::string
#include <list>

#include "Term.h"

void *loop_wrapper(void * server);
extern int errno;

class Server {
    public:

        /* ports and sockets */
        short int server_port;
        int listening_sock;
        struct sockaddr_in sin; // server in

        /* file descriptors */
        fd_set master;
        fd_set read_fds;
        int fdmax;

        /* data */
        char buffer[65536];
        std::list <String> buffers;

        /* locks */
        pthread_mutex_t buffer_lock;

        /* threads */
        pthread_t * thread;
        pthread_attr_t pthread_custom_attr; // for custom settings

        /* listening socket for server */
        void open_socket(int port_num){
            server_port = port_num;
            listening_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            if ( listening_sock < 0) {perror("socket() failed");abort();}
        }

        /* initialization - server */
        void initialize_server() {
            memset(&sin, 0, sizeof(sin));
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = INADDR_ANY;
            sin.sin_port = htons(server_port);
        }

        void reuse_socket() {
            int optval = 1;
            int sockopt = setsockopt(listening_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
            if (sockopt< 0) {perror("reuse failed");abort();}
        }

        /* server only: bind(): binds socket with a particular port number.  */
        void bind_socket() {
            int binder = bind(listening_sock, (struct sockaddr *) &sin, sizeof(sin));
            if (binder < 0) {perror("bind failed");abort();}
        }

        /* listen(): converts an active socket to passive server only listen_queue: connection‐waiting queue size. (e.g., 32) */
        void listen_socket(int queue_size) {
            int lst = listen(listening_sock, queue_size);
            if (lst < 0) {perror("listen failed");abort();}
        }

        /* init sets and add listener to the master list of socket descriptors */
        void init_fds() {
            fdmax = listening_sock;
            FD_ZERO(&master);
            FD_ZERO(&read_fds);
            FD_SET(listening_sock, &master);
        }

        /* handle a new connection */
        void new_connection(struct sockaddr_in *client_sin, int &client_sock) {

            socklen_t addr_len = sizeof(client_sin);
            client_sock = accept(listening_sock, (struct sockaddr *) client_sin, &addr_len);
            if (client_sock < 0) {perror("accept failed");abort();}
            FD_SET(client_sock, &master); /* add to master */
            if (client_sock > fdmax) fdmax = client_sock; /* set max */
            printf("client socket accepted: %d\n", client_sock);

        }

        void close_connection(int fd) {
            FD_CLR(fd, &master);
            printf("socket %d closed connection...\n", fd);
            close(fd);
        }

        void read_from_client(int fd) {
            /* read data from client */
            int expected_data_len = sizeof(buffer);
            int read_bytes = recv(fd, buffer, expected_data_len, 0);

            if (read_bytes == 0) { 
                /* connection closed */
                close_connection(fd);
            } else if (read_bytes < 0) {
                perror("recv failed");
                close_connection(fd);
            } else {
                // there must be data
                printf("received: %s\n", buffer);
            }

            buffers.push_back(String(buffer));
            memset(buffer, '\0', read_bytes);
        }

        void main_loop() {

            /* receiving data: client + server */
            int client_sock;
            struct sockaddr_in client_sin;

            /* set up time out for select */
            struct timeval tv;

            while (true) { // main outer loop
                tv.tv_sec = 5;
                tv.tv_usec = 500000;     
                read_fds = master; /* copy master */

                int sel = select(fdmax+1, &read_fds, NULL, NULL, &tv);

                if (sel == -1) {
                    perror("select()");abort();
                } else if (sel) {

                    /* DATA IS READY */
                    for(int i = 0; i <= fdmax; i++) {
                        if (FD_ISSET(i, &read_fds)) {
                            if (i == listening_sock) {
                                new_connection(&client_sin, client_sock);
                            } else {

                                pthread_mutex_lock(&buffer_lock);
                                read_from_client(i);
                                pthread_mutex_unlock(&buffer_lock);

                            } /* end read data from client */
                        } /* end if FD_SET */
                    } /* end loop through all fds */

                } else {
                    //printf("server: no data within %d seconds\n", (int) tv.tv_sec);
                    continue;
                }

                flush();

            } /* end while loop */

        }

        void start_server(int port) {

            open_socket(port);
            initialize_server();
            reuse_socket();
            bind_socket();
            listen_socket(32);
            init_fds();

            /* init mutex */
            pthread_mutex_init(&buffer_lock, NULL);

            /* init thread */
            thread = (pthread_t *) malloc(sizeof(pthread_t));
            pthread_attr_init(&pthread_custom_attr);            
            pthread_create(thread, &pthread_custom_attr, loop_wrapper, this);

        }

        String flush_buffer() {
            pthread_mutex_lock(&buffer_lock);
            String retval = String();
            for(std::list<String>::iterator it=buffers.begin(); it != buffers.end(); it++) 
                retval += *it;
            buffers.clear();
            pthread_mutex_unlock(&buffer_lock);
            return retval;
        }

        void flush() {
            pthread_mutex_lock(&buffer_lock);
            String retval = String();
            for(std::list<String>::iterator it=buffers.begin(); it != buffers.end(); it++) 
                Term::parse(*it, false);
            buffers.clear();
            pthread_mutex_unlock(&buffer_lock);
        }

        void stop_server(int port) {
            for(int i = 0; i <= fdmax; i++) {
                if (FD_ISSET(i, &read_fds)) {
                    close_connection(i);
                }
            }
            close(listening_sock);
           // pthread_detach(*thread);
           // if (thread) free(thread);
        }

        ~Server() {
            printf("stopping server\n");
            stop_server(server_port);
        }

};

