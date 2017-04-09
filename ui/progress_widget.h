#ifndef UI_PROGRESS_WIDGET_H_
#define UI_PROGRESS_WIDGET_H_

#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QWidget>

struct Task {
  // QFutureWatcher<void>* watcher;
  QString name;
  QWidget* widget;
};

class ProgressWidget : public QWidget {
 public:
  ProgressWidget(QWidget* parent);

  void AddTask(QString task_name);

  ~ProgressWidget();

 private:
  // The method checks if the widget should be visible or not.
  void CheckVisibility();

  QVector<Task> tasks_;
};

#endif  // UI_PROGRESS_WIDGET_H_
