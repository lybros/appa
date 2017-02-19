#include "newproject.h"
#include "ui_newproject.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

void NewProjectDialog::SetActiveProject(Project *active_project)
{
    project = active_project;
}

void NewProjectDialog::accept()
{
    if (create_new_project()) {
        std::cout << "created successfully!" << std::endl;
        QDialog::accept();
    } else {
        std::cout << "failed to create new project" << std::endl;
    }
}

bool NewProjectDialog::create_new_project() {
    if (verify_new_project_options()) {
        initialize_new_project();
        return true;
    } else {
        return false;
    }
}

bool NewProjectDialog::verify_new_project_options()
{
    QString error = "";

    QString projectName = ui->project_name->toPlainText();
    if (projectName.isEmpty()) {
        error = "Project Name cannot be empty";
    }

    QString projectPath = ui->project_path->text();
    if (projectPath.isEmpty()) {
        error = "Project Path cannot be empty";
    }

    QString imagePath = ui->images_path->text();
    if (imagePath.isEmpty()) {
        error = "Images Path cannot be empty";
    }

    if (!error.isEmpty()) {
        show_warning(error);
        return false;
    }
    return true;
}

bool NewProjectDialog::initialize_new_project()
{
    QString projectName = ui->project_name->toPlainText();
    QString projectPath = ui->project_path->text();
    QString imagePath = ui->images_path->text();

    project->setProjectName(projectName.toStdString());
    project->setProjectPath(projectPath.toStdString());
    project->setImagesPath(imagePath.toStdString());

    return true;
}

void NewProjectDialog::show_warning(QString& error)
{
    QMessageBox warningBox;
    warningBox.setText(error);
    warningBox.exec();
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}
