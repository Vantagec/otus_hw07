#include <iostream>
#include <sstream>
#include <thread>
#include <utility>
#include <commandscontroller.hpp>
#include <streamredirect.hpp>
#include "version.h"
//01
int main(int argc, const char *argv[]) {
    std::cout << "Bulk version: " << PROJECT_VERSION << std::endl;

    if (argc != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " [number of commands]" << std::endl;
        return 1;
    }

    const int bulkCommandsLimit = std::atoi(argv[1]);
    if (bulkCommandsLimit < 1)
        return 0;

    auto processCommands = [bulkCommandsLimit]() {
        std::string strCommand;
        bulk_defs::CommandsController cmdProc{static_cast<std::size_t>(bulkCommandsLimit)};

        while (!cmdProc.isFinished() && std::cin >> strCommand) {
            cmdProc.addCommand(std::exchange(strCommand, ""));
            using namespace std::chrono_literals;
            if (!cmdProc.isFinished())
                std::this_thread::sleep_for(1s);
        }
    };

    auto executeExample = [](const std::string& exampleName, std::istream& inputStream, auto& cmdProcessingFunc) {
        std::cout << std::endl << exampleName << ":" << std::endl;
        helpers::stream_redirect sr{std::cin, inputStream.rdbuf()};
        cmdProcessingFunc();
    };

    // Первый пример блока команд
    std::istringstream example1Stream("cmd1\ncmd2\ncmd3\ncmd4\ncmd5\nEOF");
    executeExample("example1", example1Stream, processCommands);

    // Второй пример блока команд
    std::stringstream example2Stream;
    example2Stream << "cmd1" << std::endl
                   << "cmd2" << std::endl
                   << "{" << std::endl
                   << "cmd3" << std::endl
                   << "cmd4" << std::endl
                   << "}" << std::endl
                   << "{" << std::endl
                   << "cmd5" << std::endl
                   << "cmd6" << std::endl
                   << "{" << std::endl
                   << "cmd7" << std::endl
                   << "cmd8" << std::endl
                   << "}" << std::endl
                   << "cmd9" << std::endl
                   << "}" << std::endl
                   << "{" << std::endl
                   << "cmd10" << std::endl
                   << "cmd11" << std::endl
                   << "EOF" << std::endl;
    executeExample("example2", example2Stream, processCommands);

    // Основная обработка командного ввода
    std::cout << std::endl << "enter EOF to exit or enter commands:" << std::endl;
    processCommands();

    return 0;
}

