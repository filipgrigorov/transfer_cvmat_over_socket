#include <iostream>
#include <string>

#include <arpa/inet.h>  // htons(...)
#include <netinet/in.h>  // sockaddr_in
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <opencv2/opencv.hpp>

#define IP "127.0.0.1"
#define PORT 8080

int main(void)
{
    auto sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
        throw std::runtime_error("Rx: Could not open socket!");

    //Note: Set the server ip and port to be re-used, otherwise "Already in use" might appear
    int opt = 0;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("Could not set socket options!");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << "Set the options!" << std::endl;

    if (bind(sock_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
        throw std::runtime_error("Could not bind to addr!");

    std::cout << "Binded to address!" << std::endl;

    auto n_in_queue = 3;
    if (listen(sock_fd, n_in_queue) < 0)
        throw std::runtime_error("Did not start to listen!");

    std::cout << "Started listening on port " << PORT << std::endl;

    int pair_sock_fd = -1;
    struct sockaddr_in pair_addr;
    if ((pair_sock_fd = accept(sock_fd, 
    reinterpret_cast<struct sockaddr*>(&pair_addr), 
    reinterpret_cast<socklen_t*>(&pair_addr))) < 0)
        throw std::runtime_error("Could not block for incomming connections!");

    std::cout << "Blocked and wating for connections!" << std::endl;

    const auto height = 1080;
    const auto width = 1920;

    cv::Mat frame = cv::Mat::zeros(height, width, CV_8UC3);
    const auto img_size = frame.total() * frame.elemSize();
    //const auto img_size = frame.rows * frame.cols * frame.channels();
    auto nbytes_per_row = frame.step;

    auto counter = 0;
    //uchar buffer[img_size];
    for(;;)
    {
        //process image buffers here
        auto nbytes = recv(pair_sock_fd, frame.data, img_size, MSG_WAITALL);
        std::cout << "\nReceived " << nbytes << " bytes of data!" << std::endl;

        /*for (auto row = 0; row < height; ++row)
            std::memcpy(frame.ptr<uchar>(row), buffer + row * frame.step, frame.cols * frame.elemSize());*/

        cv::imshow("Stream", frame);
        cv::imwrite("output_" + std::to_string((++counter)) + ".png", frame);

        cv::waitKey(100);

        if (nbytes == 0)
            break;
    }

    cv::destroyAllWindows();

    std::cout << "Done!" << std::endl;

    return EXIT_SUCCESS;
}
