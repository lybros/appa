// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <vector>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

#include <Eigen/Core>
#include <theia/theia.h>

#include "../src/project.h"

class ReconstructionWindow : public QOpenGLWidget, protected QOpenGLFunctions {
 public:
    ReconstructionWindow(QWidget *parent);

    void BuildWithDefaultParameters(Project *project);

    ~ReconstructionWindow();

 protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

 private:
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
