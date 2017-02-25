// Copyright 2017 Lybros.

#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <iostream>

#include <QMainWindow>
#include <QPushButton>

// to figure out what I need:
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include "../libs/QtAwesome/QtAwesome/QtAwesome.h"

#include "newproject.h"

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
    void on_actionRun_Reconstruction_triggered();

    void on_actionNewProject_triggered();

    void on_actionOpen_triggered();

    void on_actionExtract_Features_triggered();

    void on_actionMatch_Features_triggered();

    void on_actionStart_Reconstruction_triggered();

 private:
    Ui::MainWindow* ui;
    Project *active_project_;
    Qt3DExtras::Qt3DWindow *view_;
    Qt3DCore::QEntity *scene_;

    void createQt3D();

    //
    Qt3DCore::QEntity *torusEntity;
    Qt3DExtras::QTorusMesh *torusMesh;
    Qt3DCore::QTransform *torusTransform;
    Qt3DRender::QCamera *cameraEntity;
    Qt3DRender::QMaterial *material;
    QWidget *container;

    bool isProjectDirectory(QString& project_path);
};

#endif  // UI_MAINWINDOW_H_
