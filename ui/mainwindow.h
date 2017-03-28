// Copyright 2017 Lybros.

#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <iostream>

#include <QMainWindow>
#include <QProcess>
#include <QFileInfo>

#include "../libs/QtAwesome/QtAwesome/QtAwesome.h"

#include "newproject.h"
#include "reconstruction_window.h"
#include "../src/project.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);

    void set_icons(QtAwesome* awesome = 0);

    ~MainWindow();

private slots:

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

    bool isProjectDirectory(QString& project_path);

    void UpdateActiveProjectInfo();

    void EnableActions();
};

#endif  // UI_MAINWINDOW_H_
