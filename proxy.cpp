#include "proxy.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include "client_info.h"
#include "function.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void proxy::run() {
  Client_Info * client_info = new Client_Info();
  int temp_fd = build_server(this->port_num);
  int client_fd;
  while (1) {
    client_fd = server_accept(temp_fd);
    pthread_t thread;
    client_info->setFd(client_fd);
    pthread_create(&thread, NULL, handle, client_info);
  }
}

void * proxy::handle(void * info) {
  Client_Info * client_info = (Client_Info *)info;
  int client_fd = client_info->getFd();

  char req_msg[8192] = {0};
  recv(client_fd, req_msg, sizeof(req_msg), 0);
  std::cout << "received client request is:" << req_msg << std ::endl;
  std::string input = std::string(req_msg, 8192);

  Parse * parser = new Parse(input);
  const char * method = parser->method.c_str();
  std::cout << "method is " << method << "end\n";

  const char * host = parser->host.c_str();
  const char * port = parser->port.c_str();
  std::cout << "host is " << host << "end\n";
  std::cout << "port is " << port << "end\n";

  int server_fd = build_client(host, port);
  if (server_fd == -1) {
    std::cout << "Error in build client!\n";
  }

  if (parser->method == "CONNECT") {
    handleConnect(client_fd, server_fd);
  }
  else if (parser->method == "GET") {
    send(server_fd, req_msg, sizeof(req_msg), 0);
    handleGet(client_fd, server_fd);
  }
  return NULL;
}

void proxy::handleGet(int client_fd, int server_fd) {
  char server_msg[8192] = {0};
  int len = recv(server_fd, server_msg, sizeof(server_msg), 0);
  if (len == 0) {
    std::cout << "received message from server length = 0" << std::endl;
  }
  send(client_fd, server_msg, sizeof(server_msg), 0);
}

void proxy::handleConnect(int client_fd, int server_fd) {
  /*char req_msg[8192] = {0};
  if (send(server_fd, req_msg, sizeof(req_msg), MSG_NOSIGNAL) == 0) {
    std::cout << "Message send to server is 0\n";
    }*/
  /*if (recv(server_fd, mes_buf, sizeof(mes_buf), 0) == 0) {
    std::cout << "before while loop closed\n";
    }*/

  send(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19, 0);

  fd_set readfds;
  int nfds = server_fd > client_fd ? server_fd + 1 : client_fd + 1;
  while (1) {
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    FD_SET(client_fd, &readfds);
    select(nfds, &readfds, NULL, NULL, NULL);
    int fd[2] = {server_fd, client_fd};
    for (int i = 0; i < 2; i++) {
      if (FD_ISSET(fd[i], &readfds)) {
        int len;
        char message[8192] = {0};
        len = recv(fd[i], message, sizeof(message), 0);
        if (len == 0) {
          //          std::cout << "Error: Receive length 0\n";
          break;
        }
        else {
          send(fd[1 - i], message, len, 0);
          // std::cout << "Error in Sending!\n" << fd[i] << std::endl;
        }
      }
    }
  }
}
