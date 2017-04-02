// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <string>
#include <vector>

#include <QVector3D>

#include <Eigen/Core>
#include <QGLViewer/qglviewer.h>
#include <theia/theia.h>

#include "../src/project.h"

struct WorldPoint {
  QVector3D coords;
  QColor color;
};

class ModifiedCamera : public theia::Camera {
 public:
  explicit ModifiedCamera(theia::Camera camera) : theia::Camera(camera),
                                                  highlighted_(false) { }

  bool IsHighlighted() const { return highlighted_; }

  void SetHighlighted(bool highlighted) { highlighted_ = highlighted; }

  QString GetViewName() const { return view_name_; }

  void SetViewName(QString view_name) { view_name_ = view_name; }

 private:
  bool highlighted_;
  QString view_name_;
};

class ReconstructionWindow : public QGLViewer {
 public:
  ReconstructionWindow();

  explicit ReconstructionWindow(Project* project);

  void UpdateActiveProject(Project* project);

  void BuildFromDefaultPath();

  // Passing a vector of names of views to be highlighted on a map.
  // The name of the View is the same with Image name (not the full path).
  void SetHighlightedViewNames(const QVector<QString>& views);

  ~ReconstructionWindow();

 protected:
  void init() override;

  void draw() override;

 private:
  Project* project_;

  // TODO(uladbohdan): to manage the inconsistency: to use either std::vector
  // ot QVector.
  std::vector<WorldPoint> world_points_;
  std::vector<ModifiedCamera> cameras_;

  QVector<QString> highlighted_views_;

  void InitCameras(theia::Reconstruction* reconstruction);

  void UpdateHighlightedCameras();

  void DrawCamera(const ModifiedCamera&);
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
