// Copyright 2017 Lybros.

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "thumbnail_widget.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  ui->setupUi(this);
  active_project_ = new Project();

  view_ = new ReconstructionWindow(active_project_);
  ui->sceneLayout->addWidget(view_);

  ui->activeProjectInfo->setVisible(false);
  ui->imagesPreviewScrollArea->setVisible(false);
}

void MainWindow::set_icons(QtAwesome* awesome) {
  awesome->initFontAwesome();
  QVariantMap options;
// For some reasons icons with default scale-factor does not look good
// on macOS devices.
#ifdef __APPLE__
  options.insert("scale-factor", 0.5);
#endif
  // projectToolBar
  ui->actionNewProject->setIcon(awesome->icon(fa::foldero, options));
  ui->actionOpen->setIcon(awesome->icon(fa::folderopeno, options));

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
    LOG(INFO) << "New project basic parameteres:" << std::endl;
    std::cout << "\t" << project_options->project_name.toStdString()
    << std::endl;
    std::cout << "\t" << project_options->project_path.toStdString()
    << std::endl;
    std::cout << "\t" << project_options->images_path.toStdString()
    << std::endl;
    std::cout <<
    "-------------------------------------------------------------"
    << std::endl;

    // Try/catch section here to understand if constructor failed
    // to create a Project instance. (?)
    active_project_ = new Project(
        project_options->project_name,
        project_options->project_path,
        project_options->images_path);
    UpdateActiveProjectInfo();
    EnableActions();
  } else {
  }

  delete project_options;
}

void MainWindow::on_actionOpen_triggered() {
  QString projectPathChosen = QFileDialog::getExistingDirectory(
      this,
      tr("Choose the directory"),
      QDir::homePath(),
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

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

  view_->UpdateActiveProject(active_project_);

  UpdateActiveProjectInfo();
  EnableActions();
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

bool MainWindow::isProjectDirectory(const QString& project_path) {
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
  view_->BuildFromDefaultPath();
}

void MainWindow::on_actionSearch_Image_triggered() {
  LOG(INFO) << "Start search image...";
  active_project_->SearchImage(QString("images/image005.jpg"));
}

void MainWindow::on_actionRunExampleReconstruction_triggered() {
  QString output_path = active_project_->GetDefaultOutputPath();
  QString output_model_path =
      QDir(output_path).filePath(DEFAULT_MODEL_BINARY_FILENAME);

  if (!QFileInfo(output_model_path).exists()) {
    LOG(ERROR) << "No Model binary found";
    return;
  }

  QProcess view_reconstruction_process(this);
  // We assume the view_reconstruction build from Theia library is in your
  // PATH.
  view_reconstruction_process.start(
      "view_reconstruction",
      QStringList() << "--reconstruction" << output_model_path);
  view_reconstruction_process.waitForFinished();
}

void MainWindow::UpdateActiveProjectInfo() {
  if (!active_project_) {
    return;
  }

  ui->project_name_label->setText("PROJECT NAME: " +
                                  active_project_->GetProjectName());
  ui->project_location_label->setText("PROJECT LOCATION: " +
                                      active_project_->GetProjectPath());
  ui->output_location_label->setText("OUTPUT LOCATION: " +
                                     active_project_->GetOutputLocation());
  ui->images_location_label->setText("IMAGES LOCATION: " +
                                     active_project_->GetImagesPath());
  ui->number_images_label->setText("NUMBER OF IMAGES: " + QString::number(
      active_project_->GetStorage()->NumberOfImages()));

  // TODO(uladbohdan): to load in a separate thread.
  LoadImagesPreview();

  ui->activeProjectInfo->setVisible(true);
  ui->imagesPreviewScrollArea->setVisible(true);
}

void MainWindow::LoadImagesPreview() {
  for (QWidget* child : ui->imagesPreviewScrollAreaContents->
      findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
    delete child;
  }

  QVector<QString>& images = active_project_->GetStorage()->GetImages();

  if (images.empty()) {
    return;
  }

  thumbnails_.reserve(images.size());

  for (QString& image : images) {
    ThumbnailWidget* thumbnail = new ThumbnailWidget(
        this, ui->imagesPreviewScrollAreaContents, image);
    thumbnails_.push_back(thumbnail);
    ui->imagesPreviewArea->setAlignment(thumbnail, Qt::AlignHCenter);
    ui->imagesPreviewArea->addWidget(thumbnail);
  }
}

void MainWindow::UpdateSelectedThumbnails() {
  QVector<QString> view_names;
  for (auto* thumbnail : thumbnails_) {
    if (thumbnail->IsSelected()) {
      view_names.push_back(thumbnail->GetName());
    }
  }
  view_->SetHighlightedViewNames(view_names);
}

// We're enabling action buttons in case of project is loaded.
void MainWindow::EnableActions() {
  ui->actionBuildToBinary->setEnabled(true);
  ui->actionExtract_Features->setEnabled(true);
  ui->actionMatch_Features->setEnabled(true);
  ui->actionRunExampleReconstruction->setEnabled(true);
  ui->actionSearch_Image->setEnabled(true);
  ui->actionStart_Reconstruction->setEnabled(true);
  ui->actionVisualizeBinary->setEnabled(true);
}
