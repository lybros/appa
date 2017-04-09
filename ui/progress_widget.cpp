#include "progress_widget.h"

ProgressWidget::ProgressWidget(QWidget* parent) : QWidget(parent) {
  setLayout(new QVBoxLayout(this));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

  CheckVisibility();
}

ProgressWidget::~ProgressWidget() {
}

void ProgressWidget::CheckVisibility() {
  setVisible(!tasks_.empty());
}
