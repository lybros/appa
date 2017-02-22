#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    active_project = new Project();
}

void MainWindow::on_actionRun_Reconstruction_triggered()
{
    std::cout << "Reconstruction started..." << std::endl;
    active_project->RunReconstruction();
}

void MainWindow::on_actionNewProject_triggered()
{
    std::cout << "Opening New Project dialog..." << std::endl;
    NewProjectDialog new_project_dialog;
    new_project_dialog.SetActiveProject(active_project);
    if (new_project_dialog.exec()) {
        // Checking if we've initialized new project.
        std::cout << "New project basic parameteres:" << std::endl;
        std::cout << active_project->GetProjectName().toStdString() << std::endl;
        std::cout << active_project->GetProjectPath().toStdString() << std::endl;
        std::cout << active_project->GetImagesPath().toStdString() << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        QDir projectDir(active_project->GetProjectPath());
        if (!projectDir.mkdir(active_project->GetProjectName())) {
            QMessageBox warningBox;
            warningBox.setText("Failed to create a folder for project.");
            warningBox.exec();
            return;
        }
        if (!active_project->WriteConfigurationFile()) {
            QMessageBox warningBox;
            warningBox.setText("Failed to write the configuration on filesystem:(");
            warningBox.exec();
            return;
        }
    } else {
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete active_project;
}
