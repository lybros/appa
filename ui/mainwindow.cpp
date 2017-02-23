#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    active_project_ = new Project();
}

void MainWindow::on_actionRun_Reconstruction_triggered()
{
    std::cout << "Reconstruction started..." << std::endl;
    active_project_->RunReconstruction();
}

void MainWindow::on_actionNewProject_triggered()
{
    std::cout << "Opening New Project dialog..." << std::endl;
    NewProjectDialog new_project_dialog;
    new_project_dialog.SetActiveProject(active_project_);
    if (new_project_dialog.exec()) {
        // Checking if we've initialized new project.
        std::cout << "New project basic parameteres:" << std::endl;
        std::cout << active_project_->GetProjectName().toStdString() << std::endl;
        std::cout << active_project_->GetProjectPath().toStdString() << std::endl;
        std::cout << active_project_->GetImagesPath().toStdString() << std::endl;
        std::cout << "-------------------------------------" << std::endl;

        QDir projectDir(active_project_->GetProjectPath());
        if (!projectDir.mkdir(active_project_->GetProjectName())) {
            QMessageBox warningBox;
            warningBox.setText("Failed to create a folder for project.");
            warningBox.exec();
            return;
        }
        if (!active_project_->WriteConfigurationFile()) {
            QMessageBox warningBox;
            warningBox.setText("Failed to write the configuration on filesystem:(");
            warningBox.exec();
            return;
        }
    } else {
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString projectPathChosen = QFileDialog::getExistingDirectory(this,
        tr("Choose the directory"), QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // TODO(uladbohdan): to check if the folder is really a project folder.

    delete active_project_;
    active_project_ = new Project();
    active_project_->SetProjectPath(projectPathChosen);
    std::cout << "PROJECT FOLDER: " <<
                 active_project_->GetProjectPath().toStdString() << std::endl;
    if (active_project_->ReadConfigurationFile()) {
        // To check the data read from config file.
        std::cout << "project READ!" << std::endl;
        std::cout << active_project_->GetProjectName().toStdString() << std::endl;
        std::cout << active_project_->GetProjectPath().toStdString() << std::endl;
        std::cout << active_project_->GetImagesPath().toStdString() << std::endl;
        std::cout << "-------------------------------------" << std::endl;
    } else {
        std::cerr << "Reading failed!" << std::endl;
    }
}

void MainWindow::on_actionExtract_Features_triggered() {
    active_project_->ExtractFeatures();
}

void MainWindow::on_actionMatch_Features_triggered() {
    active_project_->MatchFeatures();
}

void MainWindow::on_actionStart_Reconstruction_triggered() {
    active_project_->StartReconstruction();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete active_project_;
}
