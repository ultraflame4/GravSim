#include "GravSim/logging.hh"
#include "GravSim/window.hh"


int main() {
    auto logger = logging::get("main");
    logger->info("Hello world!");

    std::shared_ptr<Window> window = std::make_shared<Window>(1000,800,"GravSim");

    window->run();


    return 0;
}
