#include <iostream>
#include "GravSim/logging.hh"

int main() {
    auto logger = logging::get("main");
    logger->info("Hello world!");
    return 0;
}
