// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <QGLViewer/qglviewer.h>

#include <QVector3D>

#include <theia/theia.h>

#include "../src/project.h"

class ReconstructionWindow : public QGLViewer {
 public:
    ReconstructionWindow();

    ReconstructionWindow(Project *project);

    void UpdateActiveProject(Project *project);

    void BuildFromDefaultPath();

    ~ReconstructionWindow();

 protected:
    virtual void init() override;
    virtual void draw() override;

 private:
    Project *project_;

    std::vector<QVector3D> world_points_;
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
