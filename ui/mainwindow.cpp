// Copyright 2017 Lybros.

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "thumbnail_widget.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          active_project_(nullptr) {
  ui->setupUi(this);

  view_ = new ReconstructionWindow();
  ui->sceneLayout->addWidget(view_);

  process_manager_ = new ProcessManager(this);
  ui->progressLayout->addWidget(process_manager_);

  ui->activeProjectInfo->setVisible(false);
  ui->imagesPreviewScrollArea->setVisible(false);
}

void MainWindow::SetIcons(QtAwesome* awesome) {
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
  ui->actionStart_Reconstruction->setIcon(awesome->icon(fa::codepen, options));

  // visualizationToolBar
  options.insert("color", QColor(31, 72, 165));
  ui->actionVisualizeBinary->setIcon(awesome->icon(fa::eye, options));

  // mainToolBar
  options.insert("color", QColor(32, 33, 35));
  ui->actionSearch_Image->setIcon(awesome->icon(fa::search, options));
  options.insert("color", QColor(67, 205, 128));
  ui->actionBuildToBinary->setIcon(awesome->icon(fa::play, options));
}

void MainWindow::on_actionBuildToBinary_triggered() {
  OptionsDialog options_dialog(this, active_project_->GetOptions(),
                               GENERAL_OPTIONS | EXTRACTING_FEATURES_OPTIONS |
                               MATCHING_FEATURES_OPTIONS |
                               RECONSTRUCTION_OPTIONS);
  if (!options_dialog.exec()) {
    return;
  }

  process_manager_->StartNewProcess(
      QString("Building to binary..."),
      QtConcurrent::run(active_project_, &Project::BuildModelToBinary));

  LOG(INFO) << "Reconstruction started...";
}

void MainWindow::on_actionNewProject_triggered() {
  NewProjectDialog new_project_dialog;
  NewProjectOptions project_options;
  new_project_dialog.SetProjectOptions(&project_options);

  if (new_project_dialog.exec()) {
    // Checking if we've initialized new project.
    LOG(INFO) << "New project basic parameteres:"
              << "\n\t" << project_options.project_name.toStdString()
              << "\n\t" << project_options.project_parent_path.toStdString()
              << "\n\t" << project_options.images_path.toStdString();

    // Try/catch section here to understand if constructor failed
    // to create a Project instance. (?)

    delete active_project_;

    active_project_ = new Project(
        project_options.project_name,
        project_options.project_parent_path,
        project_options.images_path);

    view_->UpdateActiveProject(active_project_);

    UpdateActiveProjectInfo();
    EnableActions();
  }
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

  delete active_project_;

  active_project_ = new Project(projectPathChosen);

  // To check the data read from config file.
  LOG(INFO) << "Project read!"
            << "\n\t" << active_project_->GetProjectName().toStdString()
            << "\n\t" << active_project_->GetProjectPath().toStdString()
            << "\n\t" << active_project_->GetImagesPath().toStdString();

  view_->UpdateActiveProject(active_project_);

  UpdateActiveProjectInfo();
  EnableActions();
}

void MainWindow::on_actionExtract_Features_triggered() {
  OptionsDialog options_dialog(this, active_project_->GetOptions(),
                               GENERAL_OPTIONS | EXTRACTING_FEATURES_OPTIONS);
  if (!options_dialog.exec()) {
    return;
  }

  process_manager_->StartNewProcess(
      QString("Extracting features..."),
      QtConcurrent::run(active_project_, &Project::ExtractFeatures));
}

void MainWindow::on_actionMatch_Features_triggered() {
  OptionsDialog options_dialog(this, active_project_->GetOptions(),
                               GENERAL_OPTIONS | MATCHING_FEATURES_OPTIONS);
  if (!options_dialog.exec()) {
    return;
  }

  process_manager_->StartNewProcess(
      QString("Matching features..."),
      QtConcurrent::run(active_project_, &Project::MatchFeatures));
}

void MainWindow::on_actionStart_Reconstruction_triggered() {
  OptionsDialog options_dialog(this, active_project_->GetOptions(),
                               GENERAL_OPTIONS | RECONSTRUCTION_OPTIONS);
  if (!options_dialog.exec()) {
    return;
  }

  process_manager_->StartNewProcess(
      QString("Reconstructing..."),
      QtConcurrent::run(active_project_, &Project::StartReconstruction));
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
  for (auto thumbnail : thumbnails_) {
    delete thumbnail;
  }
  thumbnails_.clear();
  delete ui;
  delete active_project_;
  delete view_;
}

void MainWindow::on_actionVisualizeBinary_triggered() {
  theia::Reconstruction* model = GetModel();
  LOG(INFO) << model->NumViews();
  view_->Visualize(model);
}

void MainWindow::on_actionSearch_Image_triggered() {
  QString image_path = QFileDialog::getOpenFileName(
      this,
      tr("Choose image for search"),
      active_project_->GetImagesPath(),
      tr("images (*.jpg *.jpeg *.png)"),
      0,
      QFileDialog::DontUseNativeDialog | QFileDialog::ReadOnly);

  if (!image_path.length()) { return; }

//  view_->Visualize();

  std::function<void(QList<QSet<theia::TrackId>*>)> on_finish =
      [this](QList<QSet<theia::TrackId>*> found_tracks) {   // why QList??
        if (!found_tracks.size()) { return; }
        view_->SetFoundPoints(found_tracks[0]);
        delete found_tracks[0];
      };

  process_manager_->StartNewProcess(
      QString("Search image " + image_path + "..."),
      QtConcurrent::run(
          active_project_,
          &Project::SearchImage,
          image_path),
      on_finish);
}

void MainWindow::UpdateActiveProjectInfo() {
  if (!active_project_) { return; }

  ui->project_name_label->setText("PROJECT NAME: " +
                                  active_project_->GetProjectName());
  ui->project_location_label->setText("PROJECT LOCATION: " +
                                      active_project_->GetProjectPath());
  ui->output_location_label->setText(
      "OUTPUT LOCATION: " + active_project_->GetStorage()->GetOutputLocation());
  ui->images_location_label->setText("IMAGES LOCATION: " +
                                     active_project_->GetImagesPath());
  ui->number_images_label->setText("NUMBER OF IMAGES: " + QString::number(
      active_project_->GetStorage()->NumberOfImages()));

  LoadImagesPreview();

  ui->activeProjectInfo->setVisible(true);
}

void MainWindow::LoadImagesPreview() {
  for (auto thumbnail : thumbnails_) {
    delete thumbnail;
  }
  thumbnails_.clear();
  ui->imagesPreviewScrollArea->setVisible(false);

  // Getting a list of image paths.
  QVector<QString>& image_paths = active_project_->GetStorage()->GetImages();
  if (image_paths.empty()) {
    return;
  }

  thumbnails_.reserve(image_paths.size());

  // Every thumbnail requires Pixmap and Label to be rendered.
  typedef QPair<QString, QPixmap> ThumbnailData;

  // The value is required to resize images in appropriate way.
  const int PREVIEW_AREA_WIDTH =
      ui->imagesPreviewScrollAreaContents->size().width() - 25;

  std::function<ThumbnailData(const QString&)> scale_images =
      [PREVIEW_AREA_WIDTH](const QString& image_path) -> ThumbnailData {
        const int INF = 99999999;
        return ThumbnailData(image_path, QPixmap::fromImage(
            QImage(image_path).scaled(PREVIEW_AREA_WIDTH, INF,
                                      Qt::KeepAspectRatio)));
      };

  std::function<void(QList<ThumbnailData>)> on_finish =
      [this](QList<ThumbnailData> pairs) {
        LOG(INFO) << "Images have been successfully loaded from filesystem.";
        ui->imagesPreviewScrollArea->setVisible(true);
        for (const ThumbnailData& pair : pairs) {
          ThumbnailWidget* thumbnail = new ThumbnailWidget(
              this, ui->imagesPreviewScrollAreaContents,
              pair.first, pair.second);
          thumbnails_.push_back(thumbnail);
          ui->imagesPreviewArea->setAlignment(thumbnail, Qt::AlignHCenter);
          ui->imagesPreviewArea->addWidget(thumbnail, 0, Qt::AlignTop);
        }
      };

  process_manager_->StartNewProcess(
      QString("Loading and rescaling thumbnails..."),
      QtConcurrent::mapped(image_paths, scale_images), on_finish);
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
  // TODO(uladbohdan): to enable actions as far as they are implemented.
  ui->actionBuildToBinary->setEnabled(true);
  ui->actionExtract_Features->setEnabled(true);
  // ui->actionMatch_Features->setEnabled(true);
  ui->actionSearch_Image->setEnabled(true);
  // ui->actionStart_Reconstruction->setEnabled(true);
  ui->actionVisualizeBinary->setEnabled(true);
}

theia::Reconstruction* MainWindow::GetModel() {
  QStringList full_names = active_project_->GetStorage()->GetReconstructions();

  if (full_names.empty()) {
    QMessageBox::warning(
        this, "No models available",
        "No models were found in filesystem. "
            "Start reconstruction process to create a new one!",
        QMessageBox::Ok);
    return NULL;
  }

  QStringList short_names;
  for (auto& full_name : full_names) {
    short_names << FileNameFromPath(full_name);
  }

  bool ok;
  QString reconstruction_name =
      QInputDialog::getItem(
          this, "Reconstruction picker",
          "Choose a reconstruction",
          short_names, 0, false, &ok);

  if (!ok || (reconstruction_name == "")) {
    LOG(WARNING) << "Failed to choose a model.";
    return NULL;
  }

  return active_project_->GetStorage()->GetReconstruction(
      full_names[short_names.indexOf(QRegExp(reconstruction_name))]);
}
