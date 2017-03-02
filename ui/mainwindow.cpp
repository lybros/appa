// Copyright 2017 Lybros.

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
    ui->setupUi(this);
    active_project_ = new Project();

    view_ = new ReconstructionWindow(this);
    ui->sceneLayout->addWidget(view_);
}

void MainWindow::set_icons(QtAwesome* awesome) {
    awesome->initFontAwesome();
    QVariantMap options;
// For some reasons icons with default scale-factor does not look good
// on macOS devices.
#ifdef __APPLE__
    options.insert("scale-factor", 0.6);
#endif

    // featuresToolBar
    options.insert("color", QColor(255, 175, 24));
    ui->actionExtract_Features->setIcon(awesome->icon(fa::dotcircleo, options));
    options.insert("color", QColor(235, 78, 78));
    ui->actionMatch_Features->setIcon(awesome->icon(fa::sharealt, options));
    options.insert("color", QColor(67, 205, 128));
    ui->actionStart_Reconstruction->setIcon(
            awesome->icon(fa::codepen, options));

    // visualizationToolBar
    options.insert("color", QColor(31, 72, 165));
    ui->actionVisualizeBinary->setIcon(awesome->icon(fa::eye, options));
    options.insert("color", QColor(96, 125, 193));
    ui->actionRunExampleReconstruction->setIcon(
            awesome->icon(fa::windowrestore, options));

    // mainToolBar
    options.insert("color", QColor(32, 33, 35));
    ui->actionSearch_Image->setIcon(awesome->icon(fa::search, options));
    options.insert("color", QColor(67, 205, 128));
    ui->actionBuildToBinary->setIcon(awesome->icon(fa::play, options));
}

void MainWindow::on_actionBuildToBinary_triggered() {
    LOG(INFO) << "Reconstruction started...";
    active_project_->BuildModelToBinary();
}

void MainWindow::on_actionNewProject_triggered() {
    NewProjectDialog new_project_dialog;
    NewProjectOptions* project_options = new NewProjectOptions();
    new_project_dialog.SetProjectOptions(project_options);

    if (new_project_dialog.exec()) {
        // Checking if we've initialized new project.
        LOG(INFO) << "New project basic parameteres:"
        << std::endl;
        std::cout << "\t" << project_options->project_name.toStdString()
        << std::endl;
        std::cout << "\t" << project_options->project_path.toStdString()
        << std::endl;
        std::cout << "\t" << project_options->images_path.toStdString()
        << std::endl;
        std::cout <<
        "-------------------------------------------------------------"
        << std::endl;

        if (active_project_) {
            delete active_project_;
        }
        // Try/catch section here to understand if constructor failed
        // to create a Project instance. (?)
        active_project_ = new Project(
                project_options->project_name,
                project_options->project_path,
                project_options->images_path);
    } else {
    }

    delete project_options;
}

void MainWindow::on_actionOpen_triggered() {
    QString projectPathChosen = QFileDialog::getExistingDirectory(
            this,
            tr("Choose the directory"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (!isProjectDirectory(projectPathChosen)) {
        QMessageBox warningBox;
        warningBox.setText("Failed to open a project.");
        warningBox.exec();

        return;
    }

    // delete active_project_;
    active_project_ = new Project();
    active_project_->SetProjectPath(projectPathChosen);

    if (!active_project_->ReadConfigurationFile()) {
        LOG(ERROR) << "Reading config file failed!";
    }

    // To check the data read from config file.
    LOG(INFO) << "Project read!";
    std::cout << "\t" << active_project_->GetProjectName().toStdString()
    << std::endl;
    std::cout << "\t" << active_project_->GetProjectPath().toStdString()
    << std::endl;
    std::cout << "\t" << active_project_->GetImagesPath().toStdString()
    << std::endl;
    std::cout << "-------------------------------------------------------------"
    << std::endl;
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

bool MainWindow::isProjectDirectory(QString& project_path) {
    QFileInfo projectDir(project_path);
    if (!projectDir.exists()) {
        LOG(ERROR) << "Failed to open the project: directory does not exist";
        return false;
    }

    QFileInfo configFileInfo(QDir(project_path).filePath("project-config"));
    if (!configFileInfo.exists()) {
        LOG(ERROR) << "Failed to open the project: no config file";
        return false;
    }

    return true;
}

MainWindow::~MainWindow() {
    delete ui;
    delete active_project_;
    delete view_;
}

void MainWindow::on_actionVisualizeBinary_triggered() {
    view_->BuildWithDefaultParameters(active_project_);
}

void MainWindow::on_actionSearch_Image_triggered() {
    LOG(INFO) << "Start search image...";
    active_project_->SearchImage(QString("images/image005.jpg"));
}

void MainWindow::on_actionRunExampleReconstruction_triggered() {
    QString output_path = active_project_->GetDefaultOutputPath();
    QString output_model_path = QDir(output_path).filePath("model-0.binary");

    if (!QFileInfo(output_model_path).exists()) {
        LOG(ERROR) << "No Model binary found";
        return;
    }

    QProcess view_reconstruction_process(this);
    view_reconstruction_process.start(
            "view_reconstruction",
            QStringList() << "--reconstruction" << output_model_path
    );
    view_reconstruction_process.waitForFinished();
}
