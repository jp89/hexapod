//#ifndef _LOGGING_H_
//#define _LOGGING_H_

//#include <iostream>






//class Logger {

//public:
//    static Logger& getInstance (void) {
//        static Logger instance;
//        return instance;
//    }
//    ~Logger() {
//    }
//    Logger(const Logger&) = delete;
//    void operator =(const Logger& ) = delete;

//    void setLogLevel(int level) {
//        this->logLevel = level;
//    }

//    template <typename T>
//    void operator<<(const T& x) {
//        if (this->logLevel != 0) {
//            std::cout << x;
//        }
//    }
//private:
//    Logger(void) {
//        this->logLevel = 0;
//    }
//    int logLevel;
//};

//#define LOG Logger::getInstance()
////#define SET_LOG_LEVEL_NONE  Logger::getInstance().setLogLevel(0);
////#define SET_LOG_LEVEL_NORMAL Logger::getInstance().setLogLevel(1);
////#define SET_LOG_LEVEL_DEBUG Logger::getInstance().setLogLevel(2);

//#endif // _LOGGING_H_
