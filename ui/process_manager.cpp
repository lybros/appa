// Copyright 2017 Lybros.

#include "process_manager.h"

int ProcessManager::next_task_id_ = 0;

ProcessManager::ProcessManager(QWidget* parent) : QWidget(parent) {
  setLayout(new QVBoxLayout(this));
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
  layout()->setMargin(0);

  // Adding horizontal separator on top of progressbar-area.
  QWidget *horizontalLineWidget = new QWidget(this);
  horizontalLineWidget->setFixedHeight(1);
  horizontalLineWidget->setSizePolicy(QSizePolicy::Expanding,
                                      QSizePolicy::Fixed);
  horizontalLineWidget->setStyleSheet(QString("background-color: #c0c0c0;"));
  layout()->addWidget(horizontalLineWidget);

  CheckVisibility();
}

ProcessManager::~ProcessManager() {
}

void ProcessManager::CheckVisibility() {
  setVisible(!tasks_.empty());
}

void ProcessManager::RemoveTaskById(int id) {
  for (int i = 0 ; i < tasks_.size(); i++) {
    if (tasks_[i].id == id) {
      delete tasks_[i].widget;
      tasks_.erase(tasks_.begin() + i);
      break;
    }
  }
  CheckVisibility();
}

int ProcessManager::NextTaskId() {
  next_task_id_++;
  return next_task_id_ - 1;
}
