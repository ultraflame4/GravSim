#include "GravSim/logging.hh"
#include <memory>
#include <iostream>
#include <chrono>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/wincolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>


void logging::Init() {
    initialised = true;

    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sinks.push_back(console);

#ifdef _WIN32
    ((std::shared_ptr<spdlog::sinks::wincolor_stdout_sink_mt>)console)->set_color(spdlog::level::trace, 0x0008);
#else
    ((std::shared_ptr<spdlog::sinks::ansicolor_stdout_sink_mt >)console)->set_color(spdlog::level::trace, "\033[100m");
#endif
    const std::chrono::zoned_time cur_time{ std::chrono::current_zone(),
                                            std::chrono::system_clock::now() };


    const std::string logfile_name = std::format("./logs/{:%d-%m-%Y_%H-%M-%OS}_log.txt", cur_time);

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt >(logfile_name));

    get<logging>()->info("Initialised logging.");
}