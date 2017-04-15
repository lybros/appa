// Copyright 2017 Lybros.

#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <iostream>

#include <QtAlgorithms>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>

#include <QFileInfo>
#include <QMainWindow>
#include <QPair>
#include <QProcess>
#include <QSet>

#include "../libs/QtAwesome/QtAwesome/QtAwesome.h"

#include "newproject.h"
#include "options_dialog.h"
#include "process_manager.h"
#include "reconstruction_window.h"

#include "../src/project.h"
#include "../src/utils.h"

class ThumbnailWidget;

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
 Q_OBJECT   // NOLINT(whitespace/indent)

 public:
  explicit MainWindow(QWidget* parent = 0);

  void set_icons(QtAwesome* awesome = 0);

  void UpdateSelectedThumbnails();

  ~MainWindow();

 private slots:   // NOLINT(whitespace/indent)

  void on_actionBuildToBinary_triggered();

  void on_actionNewProject_triggered();

  void on_actionOpen_triggered();

  void on_actionExtract_Features_triggered();

  void on_actionMatch_Features_triggered();

  void on_actionStart_Reconstruction_triggered();

  void on_actionVisualizeBinary_triggered();

  void on_actionSearch_Image_triggered();

  void on_actionRunExampleReconstruction_triggered();

 private:
  Ui::MainWindow* ui;
  Project* active_project_;

  // Interaction with widget which renders the model.
  ReconstructionWindow* view_;

  // The process_manager_ object keeps track of every passed process and
  // visualizes the progress.
  ProcessManager* process_manager_;

  bool isProjectDirectory(const QString& project_path);

  // Loads Images thumbnails to Image Preview Area.
  void LoadImagesPreview();

  QVector<ThumbnailWidget*> thumbnails_;

  void UpdateActiveProjectInfo();

  void EnableActions();
};

#endif  // UI_MAINWINDOW_H_
