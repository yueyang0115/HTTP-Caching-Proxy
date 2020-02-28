#include "parse.h"

#include <cstring>
#include <exception>
#include <iostream>
void Request::ParseLine() {
  size_t pos = input.find_first_of("\r\n");
  line = input.substr(0, pos);
}
void Request::ParseMethod() {
  size_t method_end = input.find_first_of(" ");
  method = input.substr(0, method_end);
}

void Request::ParseInput() {
  try {
    size_t pos = input.find("Host: ");
    std::string after_host = input.substr(pos + 6);
    //std::cout<<"after_host is:\n"<<after_host<<std::endl;
    size_t host_line_end;
    //std::cout << "after host charcter is:\n " << after_host[pos + 6] << std:: endl;
    host_line_end = after_host.find_first_of("\r\n");
    std::string host_line = after_host.substr(0, host_line_end);
    //std::cout << "host_line is:\n " << host_line << std:: endl;
    size_t port_begin;
    if ((port_begin = host_line.find_first_of(":\r")) != std::string::npos) {
      //std::cout << "before port number is:\n " << port_begin << std::endl;
      host = after_host.substr(0, port_begin);
      //std::cout<<"host ="<<host<<std::endl;
      //size_t port_end = after_host.find_first_of("\r\n");
      port = host_line.substr(port_begin + 1);
    }
    else {
      //std::cout << "\n cannnot find port!" << std::endl;
      // size_t port_end = after_host.find_first_of("\r\n");
      host = host_line;
      //std::cout<<"host = "<<host<<std::endl;
      port = "80";
    }
  }
  catch (std::exception & e) {
    std::cout << "Host is Empty!" << std::endl;
    host = "";
    port = "";
    return;
  }
}
