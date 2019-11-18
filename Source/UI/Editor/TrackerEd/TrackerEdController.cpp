#include "TrackerEdController.h"

#include <imgui.h>


TrackerEdController::TrackerEdController() {

}

TrackerEdController::~TrackerEdController() {

}


void TrackerEdController::Init() {
    this->view.Init();
}

void TrackerEdController::DeInit() {
    this->view.DeInit();
}

void TrackerEdController::Render() {
    this->view.Render();
}
