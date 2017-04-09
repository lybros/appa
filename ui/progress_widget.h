#ifndef UI_PROGRESS_WIDGET_H_
#define UI_PROGRESS_WIDGET_H_

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QWidget>

//template<typename T>
struct Task {
 // QFutureWatcher<T>* watcher;
  QString name;
  QWidget* widget;
};

class ProgressWidget : public QWidget {
 public:
  ProgressWidget(QWidget* parent);

  template<typename T>
  void AddTask(QString task_name, QFutureWatcher<T>* watcher)  {
    QWidget* new_progress_tracker = new QWidget(this);
    new_progress_tracker->setSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Maximum);

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

    connect(watcher, &QFutureWatcher<T>::finished, [=](){
      for (int i = 0 ; i < tasks_.size(); i++) {
        if (tasks_[i].widget == new_progress_tracker) {
          tasks_.erase(tasks_.begin() + i);
          delete new_progress_tracker;
          break;
        }
      }
      CheckVisibility();
    });

    CheckVisibility();
  }

  ~ProgressWidget();

 private:
  // The method checks if the widget should be visible or not.
  void CheckVisibility();

  QVector<Task> tasks_;
};

#endif  // UI_PROGRESS_WIDGET_H_
