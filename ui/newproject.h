#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <QDialog>

#include "../src/project.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    void SetActiveProject(Project *project);
    ~NewProjectDialog();

private slots:
    void accept();

private:
    Ui::NewProjectDialog *ui;
    Project *project;

    bool verify_new_project_options();
    bool initialize_new_project();
    void show_warning();
};

#endif // NEWPROJECT_H
