#if defined(LOGGER_ASYNC)
#else
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <Windows.h>

#include "logger.hpp"

class Logger {
private:
    bool log_to_console;
    std::ofstream log_file;
public:
    Logger(bool create_console) : log_to_console(create_console) {}

    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    bool init_file_logging(const std::filesystem::path& file_path) {
        log_file.open(file_path);
        if (!log_file.is_open()) {
            return false;
        }
        return true;
    }

    void log(const std::string& str) {
        if (log_to_console) {
            std::cout << str << std::endl;
        }
        if (log_file.is_open()) {
            log_file << str << std::endl;
        }
    }
};

std::unique_ptr<Logger> LOGGER{};
std::mutex LOGGER_LOCK{};

void logger::init(const std::filesystem::path& file_path, bool create_console) {
    std::scoped_lock<std::mutex> lock(LOGGER_LOCK);
    LOGGER = std::make_unique<Logger>(create_console);
    LOGGER->init_file_logging(file_path);
}

void logger::log(const std::string& str) {
    std::scoped_lock<std::mutex> lock(LOGGER_LOCK);
    LOGGER->log(str);
}
#endif
