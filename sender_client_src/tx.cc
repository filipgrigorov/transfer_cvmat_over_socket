#include <iostream>
#include <string>

#include <arpa/inet.h>  // htons(...)
#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#define IP "127.0.0.1"
#define PORT 8080

int main(int argc, char** argv)
{
    if (argc != 3)
    throw std::runtime_error("Requires 3 arguments!");

    std::string rgb_image_path{ argv[1] };
    auto rgb_frame = cv::imread(rgb_image_path, cv::IMREAD_COLOR);

    if (!rgb_frame.data)
        throw std::runtime_error("Empty image!");

    //AF_INET: IPv4
    //SOCK_STREAM: TCP uninterrupted
    auto sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        throw std::runtime_error("Tx: Could not open socket!");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);  // Converts port from host to network byte order

    if (inet_pton(AF_INET, IP, &server_addr.sin_addr) < 0)
        throw std::runtime_error("Could not turn byte order ...");

    if (connect(sock_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) < 0)
        throw std::runtime_error("Could not connect to address!");

    //auto total_contiguous_nelems = rgb_frame.total() * rgb_frame.elemSize();
    auto total_contiguous_nelems = rgb_frame.rows * rgb_frame.cols * rgb_frame.channels();
    std::cout << "Expected to send out " << total_contiguous_nelems << std::endl;

    auto n = std::stoi(argv[2]);
    for (auto idx = 0; idx < n; ++idx)
    {
        std::cout << "n: " << n << std::endl;
        auto nbytes = send(sock_fd, rgb_frame.data, total_contiguous_nelems, 0);
        if (nbytes < 0)
            continue;
        std::cout << "\nSent out " << nbytes << " bytes over socket!" << std::endl;
    }

    return EXIT_SUCCESS;
}
