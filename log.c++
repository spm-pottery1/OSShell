#include <fstream>
#include <ctime>
#include <iomanip>

struct log {
    private:
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        char buffer[32];    
    public:

    log() {}
    void logIt(const std::string& message) {
        std::ofstream logFile("log.txt", std::ios::app); 
        std::strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", &tm);
        if (logFile.is_open()) {
            logFile << buffer << " " << message << std::endl;
            logFile.close();
        }
    }
};  