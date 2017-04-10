#include "progress_widget.h"

int ProgressWidget::next_task_id_ = 0;

ProgressWidget::ProgressWidget(QWidget* parent) : QWidget(parent) {
  setLayout(new QVBoxLayout(this));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  layout()->setMargin(0);

  // Adding horizontal separator on top of progressbar-area.
  QWidget *horizontalLineWidget = new QWidget(this);
  horizontalLineWidget->setFixedHeight(1);
  horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  horizontalLineWidget->setStyleSheet(QString("background-color: #c0c0c0;"));
  layout()->addWidget(horizontalLineWidget);

  CheckVisibility();
}

ProgressWidget::~ProgressWidget() {
}

void ProgressWidget::CheckVisibility() {
  setVisible(!tasks_.empty());
}

void ProgressWidget::RemoveTaskById(int id) {
  for (int i = 0 ; i < tasks_.size(); i++) {
    if (tasks_[i].task_id == id) {
      delete tasks_[i].widget;
      tasks_.erase(tasks_.begin() + i);
      break;
    }
  }
  CheckVisibility();
}

int ProgressWidget::NextTaskId() {
  next_task_id_++;
  return next_task_id_ - 1;
}
