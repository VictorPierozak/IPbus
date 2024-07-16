#include <iostream>
#include "IPbusInterface.h"

int main() {
try {
        boost::asio::io_context io_service;
        IPbusTarget target(io_service,"127.0.0.1");
        
        for(int i = 0; i < 10; i++)
        {
            std::cout<< "Wait..." << std::endl;
            sleep(1);
        }

        //io_service.run();  // Run the io_service to process async operations
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    };
    return 0;
}
