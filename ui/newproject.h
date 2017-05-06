// Copyright 2017 Lybros.

#ifndef UI_NEWPROJECT_H_
#define UI_NEWPROJECT_H_

#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "../src/project.h"
#include "../src/utils.h"

namespace Ui { class NewProjectDialog; }

struct NewProjectOptions;

class NewProjectDialog : public QDialog {
 Q_OBJECT   // NOLINT(whitespace/indent)

 public:
  explicit NewProjectDialog(QWidget* parent = 0);

  void SetProjectOptions(NewProjectOptions* project_options);

  ~NewProjectDialog();

 private slots:   // NOLINT(whitespace/indent)

  void accept();

  void on_locate_project_button_clicked();

  void on_locate_images_button_clicked();

 private:
  Ui::NewProjectDialog* ui;
  NewProjectOptions* project_options_;

  bool create_new_project();

  bool verify_new_project_options();

  bool initialize_new_project();

  void show_warning(const QString& error);
};

struct NewProjectOptions {
  QString project_name;
  QString project_parent_path;
  QString images_path;
};

#endif  // UI_NEWPROJECT_H_
