#include "Viewport3DController.h"

Viewport3DController::Viewport3DController() : view(model) {

}

Viewport3DController::~Viewport3DController() {

}

void Viewport3DController::Init() {

}

void Viewport3DController::DeInit() {

}

void Viewport3DController::Render() {
	this->view.Render();
}
