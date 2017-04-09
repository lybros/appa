#include "progress_widget.h"

ProgressWidget::ProgressWidget(QWidget* parent) : QWidget(parent) {
  setLayout(new QVBoxLayout(this));
  CheckVisibility();
}

ProgressWidget::~ProgressWidget() {
}

void ProgressWidget::AddTask(QString task_name) {
  QWidget* new_progress_tracker = new QWidget(this);
  new_progress_tracker->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  QHBoxLayout* box_layout = new QHBoxLayout(new_progress_tracker);
  new_progress_tracker->setLayout(box_layout);

  QLabel* name_label = new QLabel(task_name, new_progress_tracker);
  box_layout->addWidget(name_label);

  QProgressBar* progress_bar = new QProgressBar(new_progress_tracker);
  box_layout->addWidget(progress_bar);
  // TODO(uladbohdan): to make progressbar non-constant.
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(100);
  progress_bar->setValue(50);

  Task new_task;
  new_task.name = task_name;
  //new_task.watcher = watcher;
  new_task.widget = new_progress_tracker;

  tasks_.push_back(new_task);

  layout()->addWidget(new_progress_tracker);

  CheckVisibility();
}

void ProgressWidget::CheckVisibility() {
  setVisible(!tasks_.empty());
}
