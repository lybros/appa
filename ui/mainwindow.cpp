// Copyright 2017 Lybros.

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    active_project_ = new Project();
    createQt3D();
}

void MainWindow::createQt3D() {
    view_ = new Qt3DExtras::Qt3DWindow;
    scene_ = new Qt3DCore::QEntity;
    container = QWidget::createWindowContainer(view_);

    ui->sceneLayout->addWidget(container);

    // Material
    material = new Qt3DExtras::QPhongMaterial(scene_);

    // Torus
    torusEntity = new Qt3DCore::QEntity(scene_);
    torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Camera
    cameraEntity = view_->camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    view_->setRootEntity(scene_);
}

void MainWindow::set_icons(QtAwesome* awesome) {
    awesome->initFontAwesome();
    QVariantMap options;

    options.insert("color", QColor(0, 189, 58));
    ui->actionRun_Reconstruction->setIcon(
                awesome->icon(fa::play, options));
    options.insert("color", QColor(255, 175, 24));
    ui->actionExtract_Features->setIcon(
                awesome->icon(fa::crosshairs, options));
    options.insert("color", QColor(147, 205, 255));
    ui->actionMatch_Features->setIcon(
                awesome->icon(fa::filepictureo, options));
    options.insert("color", QColor(0, 189, 58));
    ui->actionStart_Reconstruction->setIcon(
                awesome->icon(fa::play, options));
}

void MainWindow::on_actionRun_Reconstruction_triggered() {
    std::cout << "Reconstruction started..." << std::endl;
    active_project_->RunReconstruction();
}

void MainWindow::on_actionNewProject_triggered() {
    NewProjectDialog new_project_dialog;
    NewProjectOptions* project_options = new NewProjectOptions();
    new_project_dialog.SetProjectOptions(project_options);

    if (new_project_dialog.exec()) {
        // Checking if we've initialized new project.
        std::cout << "[MainWindow] New project basic parameteres:"
                  << std::endl;
        std::cout << project_options->project_name.toStdString()
                  << std::endl;
        std::cout << project_options->project_path.toStdString()
                  << std::endl;
        std::cout << project_options->images_path.toStdString()
                  << std::endl;
        std::cout << "-------------------------------------" << std::endl;

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
    QString projectPathChosen =
            QFileDialog::getExistingDirectory(this,
                                              tr("Choose the directory"),
                                              QDir::homePath(),
                                              QFileDialog::ShowDirsOnly |
                                              QFileDialog::DontResolveSymlinks);

    if (!isProjectDirectory(projectPathChosen)) {
        QMessageBox warningBox;
        warningBox.setText("Failed to open a project.");
        warningBox.exec();

        return;
    }

    // delete active_project_;
    active_project_ = new Project();
    active_project_->SetProjectPath(projectPathChosen);
    if (active_project_->ReadConfigurationFile()) {
        // To check the data read from config file.
        std::cout << "[MainWindow] project READ!" << std::endl;
        std::cout << active_project_->GetProjectName().toStdString()
                  << std::endl;
        std::cout << active_project_->GetProjectPath().toStdString()
                  << std::endl;
        std::cout << active_project_->GetImagesPath().toStdString()
                  << std::endl;
        std::cout << "-------------------------------------" << std::endl;
    } else {
        std::cerr << "[MainWindow] Reading config file failed!" << std::endl;
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

bool MainWindow::isProjectDirectory(QString &project_path) {
    QFileInfo projectDir(project_path);
    if (!projectDir.exists()) {
        std::cerr << "[MainWindow] Failed to open the project: directory does not exist."
                  << std::endl;
        return false;
    }

    QFileInfo configFileInfo(QDir(project_path).filePath("project-config"));
    if (!configFileInfo.exists()) {
        std::cerr << "[MainWindow] Failed to open the project: no config file."
                  << std::endl;
        return false;
    }

    return true;
}

MainWindow::~MainWindow() {
    delete ui;
    delete active_project_;
    // delete view_;
    // delete scene_;
}
