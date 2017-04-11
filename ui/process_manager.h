#ifndef UI_PROCESS_MANAGER_H_
#define UI_PROCESS_MANAGER_H_

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QWidget>

//template<typename T>
struct Task {
 // QFutureWatcher<T>* watcher;
  int task_id;
  QString name;
  QWidget* widget;
};

class ProcessManager : public QWidget {
 public:
  ProcessManager(QWidget* parent);

  template<typename T>
  void AddTask(QString task_name, QFutureWatcher<T>* watcher)  {
    QWidget* new_progress_tracker = new QWidget(this);
    new_progress_tracker->setSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Maximum);

    QHBoxLayout* box_layout = new QHBoxLayout(new_progress_tracker);
    box_layout->setMargin(0);
    new_progress_tracker->setLayout(box_layout);

    QLabel* name_label = new QLabel(task_name, new_progress_tracker);
    box_layout->addWidget(name_label);

    QProgressBar* progress_bar = new QProgressBar(new_progress_tracker);
    box_layout->addWidget(progress_bar);

    Task new_task;
    new_task.task_id = NextTaskId();
    new_task.name = task_name;
    //new_task.watcher = watcher;
    new_task.widget = new_progress_tracker;

    tasks_.push_back(new_task);

    layout()->addWidget(new_progress_tracker);

    QObject::connect(watcher, &QFutureWatcher<T>::finished, this,
            [this, new_task](){RemoveTaskById(new_task.task_id);});

    QObject::connect(watcher, SIGNAL(progressRangeChanged(int,int)),
                     progress_bar, SLOT(setRange(int,int)));
    QObject::connect(watcher, SIGNAL(progressValueChanged(int)),
                     progress_bar, SLOT(setValue(int)));

    CheckVisibility();
  }

  ~ProcessManager();

 private:
  // The method checks if the widget should be visible or not.
  void CheckVisibility();

  QVector<Task> tasks_;

  void RemoveTaskById(int id);

  static int next_task_id_;

  int NextTaskId();
};

#endif  // UI_PROCESS_MANAGER_H_
