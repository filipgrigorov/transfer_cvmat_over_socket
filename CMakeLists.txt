cmake_minimum_required(VERSION 3.5)
project(cv_mat_over_sock)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 14)

set(rx_src receiver_server_src/rx.cc)
set(tx_src sender_client_src/tx.cc)

add_executable(tx ${tx_src})
target_link_libraries(tx PUBLIC ${OpenCV_LIBS})

add_executable(rx ${rx_src})
target_link_libraries(rx PUBLIC ${OpenCV_LIBS})
