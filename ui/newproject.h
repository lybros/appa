/* The project structure is:
 * project_name_folder
 * |--"project-config"
 *
 * project-config file is:
 * ---------------------------------------------
 * PROJECT_CONFIG_VERSION v1.0
 * PROJECT_NAME "project-name"
 * IMAGES_LOCATION "~/datasets/dataset0/"
 * NUMBER_OF_IMAGES N_IMAGES
 * IMAGE_NAMES
 * "name0.jpg"
 * ...
 * "nameN.jpg"
 * OUTPUT_LOCATION "~/models/model0"
 * --------------------------------------------- */

#ifndef NEWPROJECT_H
#define NEWPROJECT_H

#include <QDialog>
#include <QMessageBox>

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

    bool create_new_project();
    bool verify_new_project_options();
    bool initialize_new_project();
    void show_warning(QString& error);
};

#endif // NEWPROJECT_H
