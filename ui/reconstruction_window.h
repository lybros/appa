// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <QGLViewer/qglviewer.h>

#include <QVector3D>

#include <Eigen/Core>
#include <theia/theia.h>

#include "../src/project.h"

struct WorldPoint {
    QVector3D coords;
    QColor color;
};

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

    std::vector<WorldPoint> world_points_;
    std::vector<theia::Camera> cameras_;

    void DrawCamera(const theia::Camera&);
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
