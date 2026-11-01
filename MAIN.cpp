#include <iostream>
#include "services.h"

int main() {
    Restaurant* R = nullptr;
    int choice = 0;
    
    while (true) {
        std::cout << "\n1.Show menu\n2.Order Food\n3.Show history\n4.Admin panel\n5.Exit" << std::endl;
        std::cin >> choice;

        if (R) delete R; // Clean up old service memory

        if (choice == 1) R = new menuservice;
        else if (choice == 2) R = new orderservice;
        else if (choice == 3) R = new historyservice;
        else if (choice == 4) R = new adminservice;
        else break;

        R->execute();
    }
    delete R;
    return 0;
}
/*to run file manual in vscode i need to copy the following command
g++ -I"./Header" MAIN.cpp ./Cpp/*.cpp -o RestaurantApp.exe
And then this: .\RestaurantApp.exe
*/