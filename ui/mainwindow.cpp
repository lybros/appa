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
        cout << "accepted" << endl;
    } else {
        cout << "rejected" << endl;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete active_project;
}
