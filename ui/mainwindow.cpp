// Copyright 2017 Lybros.

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "thumbnail_widget.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow) {
  ui->setupUi(this);

  view_ = new ReconstructionWindow();
  ui->sceneLayout->addWidget(view_);

  progress_widget_ = new ProgressWidget(this);
  ui->progressLayout->addWidget(progress_widget_);

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
  QFutureWatcher<void>* watcher = new QFutureWatcher<void>();

  progress_widget_->AddTask(QString("Building to binary..."), watcher);

  QFuture<void> future = QtConcurrent::run(active_project_,
                                           &Project::BuildModelToBinary);
  watcher->setFuture(future);

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
              << "\n\t" << project_options.project_path.toStdString()
              << "\n\t" << project_options.images_path.toStdString();

    // Try/catch section here to understand if constructor failed
    // to create a Project instance. (?)
    active_project_ = new Project(
        project_options.project_name,
        project_options.project_path,
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

  // delete active_project_;
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
  QSet<theia::TrackId>* highlighted_tracks = new QSet<theia::TrackId>();
  // TODO(drapegnik): replace hardcode with variables from dialog
  QString image =
      QDir(active_project_->GetImagesPath()).filePath("image005.jpg");
  active_project_->SearchImage(image, highlighted_tracks);
  view_->SetHighlightedPoints(highlighted_tracks);
  delete highlighted_tracks;
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
  // We assume the view_reconstruction build from Theia library is in your PATH.
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
  // Removing all old thumbnails.
  for (QWidget* child : ui->imagesPreviewScrollAreaContents->
      findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly)) {
    delete child;
  }
  ui->imagesPreviewScrollArea->setVisible(false);

  // Getting a list of image paths.
  QVector<QString>& image_paths = active_project_->GetStorage()->GetImages();
  if (image_paths.empty()) {
    return;
  }

  thumbnails_.reserve(image_paths.size());

  // Every thumbnail requires Pixmap and Label to be rendered.
  typedef QPair<QString, QPixmap> ThumbnailData;

  // Watcher keeps track of the retrieving-images-thread.
  QFutureWatcher<ThumbnailData>* watcher = new QFutureWatcher<ThumbnailData>();

  QObject::connect(watcher, &QFutureWatcher<ThumbnailData>::finished, [=](){
    LOG(INFO) << "Images have been successfully loaded from filesystem.";
    ui->imagesPreviewScrollArea->setVisible(true);

    QList<ThumbnailData> pairs = watcher->future().results();
    for (const ThumbnailData& pair : pairs) {
      ThumbnailWidget* thumbnail = new ThumbnailWidget(
        this, ui->imagesPreviewScrollAreaContents, pair.first, pair.second);
      thumbnails_.push_back(thumbnail);
      ui->imagesPreviewArea->setAlignment(thumbnail, Qt::AlignHCenter);
      // ui->imagesPreviewArea->setAlignment(thumbnail, Qt::AlignTop);
      ui->imagesPreviewArea->addWidget(thumbnail, 0, Qt::AlignTop);
    }
  });

  progress_widget_->AddTask(QString("Loading and scaling thumbnails..."),
                            watcher);

  // The value is required to resize images in appropriate way.
  const int PREVIEW_AREA_WIDTH =
      ui->imagesPreviewScrollAreaContents->size().width() - 25;

  std::function<ThumbnailData (const QString&)> scale_images =
      [PREVIEW_AREA_WIDTH](const QString& image_path)->ThumbnailData{
    const int INF = 99999999;
    return ThumbnailData(image_path, QPixmap::fromImage(
            QImage(image_path).scaled(PREVIEW_AREA_WIDTH, INF,
                                      Qt::KeepAspectRatio)));
  };

  QFuture<ThumbnailData> future =
      QtConcurrent::mapped(image_paths, scale_images);

  watcher->setFuture(future);
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
