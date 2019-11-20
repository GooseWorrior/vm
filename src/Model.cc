#include "Model.h"
#include <memory>
#include "controller/Keyboard.h"
#include "view/View.h"

using std::unique_ptr;

namespace CS246E {
Model::Model() {}
Model::~Model() {}
void Model::addController(unique_ptr<Keyboard> controller) {
  this->controller = std::move(controller);
}
void Model::addView(unique_ptr<View> view) { this->view = std::move(view); }
}  // namespace CS246E
