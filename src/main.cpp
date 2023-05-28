#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../include/Tasks.h"

int main(int argc, char** argv) {

    char in;
    std::cout << "Press enter to start:" << std::endl;
    std::cin.get(in);

    std::cout << "Task 1 press any key to continue." << std::endl;
    task1(argc, argv);

    std::cout << "Task 2 press any key to continue." << std::endl;
    task2(argc, argv);

    std::cout << "Task 3 press any key to continue." << std::endl;
    task3(argc, argv);

    std::cout << "Task 4 press any key to continue." << std::endl;
    task4(argc, argv);

    std::cout << "Task 5 press any key to continue." << std::endl;
    task5(argc, argv);

    std::cout << "Tasks terminated." << std::endl;

    return 0;
}
