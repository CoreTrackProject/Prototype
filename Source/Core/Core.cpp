#include "Core.h"

#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>



Core::Core() {}
Core::~Core() {}

void Core::Run() {

    // Configure logging

    // Output message to console
    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = "[%TimeStamp%][%Severity%]:  %Message%",
        boost::log::keywords::auto_flush = true
        );




    this->windowManager.Init();
    this->windowManager.BeginMessageLoop();
    this->windowManager.DeInit();
}
