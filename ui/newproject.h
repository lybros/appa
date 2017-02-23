// Copyright 2017 Lybros.

#ifndef UI_NEWPROJECT_H_
#define UI_NEWPROJECT_H_

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>

#include "../src/project.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT

 public:
    explicit NewProjectDialog(QWidget *parent = 0);
    void SetActiveProject(Project *project);
    ~NewProjectDialog();

 private slots:
    void accept();

    void on_locate_project_button_clicked();

    void on_locate_images_button_clicked();

 private:
    Ui::NewProjectDialog *ui;
    Project *project;

    bool create_new_project();
    bool verify_new_project_options();
    bool initialize_new_project();
    void show_warning(QString& error);
};

#endif  // UI_NEWPROJECT_H_
