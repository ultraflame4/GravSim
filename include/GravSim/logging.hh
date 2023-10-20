#pragma once

#include <string>
#include <spdlog/spdlog.h>


class logging {
public:
    typedef std::shared_ptr<spdlog::logger> logger;

    template<typename T>
    logger static get(const std::string &prefix = "") {
        if (!initialised) Init();
        std::string type_name = typeid(T).name();
        std::string logger_name = type_name + (prefix.empty() ? "" : " <" + prefix +">");

        return createLogger(logger_name);
    }


    logger static get(const std::string &name = "") {
        if (!initialised) Init();

        std::string logger_name = " <" + name +">";

        return createLogger(logger_name);
    }

private:
    static inline std::vector<spdlog::sink_ptr> sinks{};
    static inline bool initialised = false;
    static void Init();
    static logger createLogger(const std::string &logger_name = ""){
        std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(logger_name, sinks.begin(),
                                                                                  sinks.end());
        logger->set_pattern("%^%Y-%m-%d %T.%e | %=5l | %n : %v%$");
        logger->set_level(spdlog::level::trace);
        return logger;
    }
};