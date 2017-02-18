#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    project = new Project();
}

void MainWindow::on_actionRun_Reconstruction_triggered()
{
    std::cout << "Reconstruction started..." << std::endl;
    project->RunReconstruction();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete project;
}
