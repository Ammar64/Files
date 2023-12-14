#ifndef SERVER_H
#define SERVER_H
#define DEFAULT_PORT 12345

#include <httplib.h>
#include <thread>
class Server final {
  public:
    Server();
    void Start();
    void Restart();
    void Stop();
    httplib::Server svr;
    uint16_t port = DEFAULT_PORT;
  private:
    std::thread *thread = nullptr;
    void __start_server();
};

#endif