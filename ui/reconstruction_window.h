// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include <QVector3D>
#include <QSet>

#include <Eigen/Core>
#include <QGLViewer/qglviewer.h>
#include <theia/theia.h>

#include "../src/project.h"

struct WorldPoint {
  QVector3D coords;
  QColor color;
  theia::TrackId trackId;
  bool is_found;
  bool is_selected;
};

class ModifiedCamera : public theia::Camera {
 public:
  explicit ModifiedCamera(theia::Camera camera) : theia::Camera(camera),
                                                  highlighted_(false) {}

  bool IsHighlighted() const { return highlighted_; }

  void SetHighlighted(bool highlighted) { highlighted_ = highlighted; }

  QString GetViewName() const { return view_name_; }

  void SetViewName(QString view_name) { view_name_ = view_name; }

 private:
  bool highlighted_;
  QString view_name_;
};

// ReconstructionWindow class contains all methods to visualize data.
// The data comes from project_ field, which connects us with instance of
// Project class.
class ReconstructionWindow : public QGLViewer {
 Q_OBJECT   // NOLINT(whitespace/indent)

 public:
  ReconstructionWindow();

  explicit ReconstructionWindow(Project* project);

  // Sets the active app project. Must be called every time the user switches
  // the project.
  void UpdateActiveProject(Project* project);

  // Gets reconstruction and render it on screen.
  void Visualize(theia::Reconstruction* reconstruction);

  // Passing a vector of names of views to be highlighted on a map.
  // The name of the View is the same with Image name (not the full path).
  void SetHighlightedViewNames(const QVector<QString>& views);

  // Set found flag for WorldPoint
  void SetFoundPoints(const QSet<theia::TrackId>* found_tracks);

  // Set selected flag for WorldPoint
  void SetSelectedPoints(const std::multiset<theia::TrackId>& s_tracks);

  void RenderSlamExperiment(std::vector<theia::Camera>& cameras);

  ~ReconstructionWindow();

 protected:
  void init() override;

  void draw() override;

 private:
  Project* project_;
  theia::Reconstruction* reconstruction_;

  // TODO(uladbohdan): to manage the inconsistency: to use either std::vector
  // ot QVector.
  std::vector<WorldPoint> world_points_;
  std::vector<ModifiedCamera> cameras_;

  QVector<QString> highlighted_views_;

  void InitCameras();

  void UpdateHighlightedCameras();

  void DrawCamera(const ModifiedCamera&);
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
