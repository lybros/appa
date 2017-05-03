// Copyright 2017 Lybros.

#ifndef UI_PROCESS_MANAGER_H_
#define UI_PROCESS_MANAGER_H_

#include <iostream>

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTextStream>
#include <QTimer>
#include <QWidget>

struct Task {
  int id;
  QString name;
  QWidget* widget;
};

class ProcessManager : public QWidget {
 Q_OBJECT   // NOLINT(whitespace/indent)

 public:
  explicit ProcessManager(QWidget* parent);

  // Running new process without any actions after process is finished.
  template<typename T>
  void StartNewProcess(QString process_description, QFuture<T> future) {
    QFutureWatcher<T>* watcher = new QFutureWatcher<T>();
    AddTask(process_description, watcher);
    watcher->setFuture(future);
  }

  // Running new process with calling on_finish function after the process is
  // finished.
  template<typename T, typename Functor>
  void StartNewProcess(QString process_description, QFuture<T> future,
                       Functor on_finish) {
    QFutureWatcher<T>* watcher = new QFutureWatcher<T>();

    QObject::connect(watcher, &QFutureWatcher<T>::finished,
                     [=]{ on_finish(watcher->future().results()); });

    AddTask(process_description, watcher);
    watcher->setFuture(future);
  }

  ~ProcessManager();

 private:
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

    QLabel* timer_label = new QLabel("00:00", new_progress_tracker);
    box_layout->addWidget(timer_label);

    QTimer* timer = new QTimer(new_progress_tracker);
    connect(timer, &QTimer::timeout, [timer_label] {
      int cur_mins, cur_secs;
      QStringList splitted_time = timer_label->text().split(':');
      QTextStream(&splitted_time[0]) >> cur_mins;
      QTextStream(&splitted_time[1]) >> cur_secs;
      int seconds_to_show = cur_mins * 60 + cur_secs + 1;
      int mins = seconds_to_show / 60;
      int secs = seconds_to_show % 60;
      timer_label->setText(QString(mins < 10 ? "0" : "") +
                           QString::number(seconds_to_show / 60) +
                           QString(":") +
                           QString(secs < 10 ? "0" : "") +
                           QString::number(seconds_to_show % 60));
    });
    timer->start(1000);

    Task new_task;
    new_task.id = NextTaskId();
    new_task.name = task_name;
    new_task.widget = new_progress_tracker;

    tasks_.push_back(new_task);

    layout()->addWidget(new_progress_tracker);

    QObject::connect(watcher, &QFutureWatcher<T>::finished, this,
            [this, new_task](){RemoveTaskById(new_task.id);});

    QObject::connect(watcher, SIGNAL(progressRangeChanged(int, int)),
                     progress_bar, SLOT(setRange(int, int)));
    QObject::connect(watcher, SIGNAL(progressValueChanged(int)),
                     progress_bar, SLOT(setValue(int)));

    // Deallocating a watcher object after the task is finished.
    // Keep in mind, this connection must go after any other connections to
    // watcher's "finished" signal.
    QObject::connect(watcher, &QFutureWatcher<T>::finished,
                     [watcher]{ delete watcher; });

    // Canceling every process which is still alive if the app is destroyed.
    QObject::connect(this, &QWidget::destroyed,
                     [=]{ watcher->future().cancel(); });

    CheckVisibility();
  }

  // The method checks if the widget should be visible or not.
  void CheckVisibility();

  QVector<Task> tasks_;

  void RemoveTaskById(int id);

  static int next_task_id_;

  int NextTaskId();
};

#endif  // UI_PROCESS_MANAGER_H_
