// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <QGLViewer/qglviewer.h>

#include <Eigen/Core>
#include <theia/theia.h>

#include "../src/project.h"

class ReconstructionWindow : public QGLViewer {
 public:
    void BuildWithDefaultParameters(Project *project);

    ~ReconstructionWindow();

 protected:
    virtual void init() override;

    virtual void draw() override;
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
