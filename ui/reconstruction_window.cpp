// Copyright 2017 Lybros.

#include "reconstruction_window.h"

#include <QInputDialog>
#include <QMessageBox>

ReconstructionWindow::ReconstructionWindow() : reconstruction_(nullptr) {
  QGLViewer();
}

ReconstructionWindow::ReconstructionWindow(Project* project) {
  ReconstructionWindow();
  project_ = project;
}

void ReconstructionWindow::UpdateActiveProject(Project* project) {
  project_ = project;
}

void ReconstructionWindow::BuildFromDefaultPath() {
  world_points_.clear();

  QStringList full_names = project_->GetStorage()->GetReconstructions();
  if (full_names.empty()) {
    QMessageBox::warning(
        this, "No models available",
        "No models were found in filesystem. "
            "Start reconstruction process to create a new one!",
        QMessageBox::Ok);
    return;
  }

  QStringList short_names;
  for (auto& full_name : full_names) {
    short_names << FileNameFromPath(full_name);
  }

  bool ok;
  QString reconstruction_name =
      QInputDialog::getItem(
          this, "Reconstruction picker",
          "Choose a reconstruction to render",
          short_names, 0, false, &ok);

  if (!ok || (reconstruction_name == "")) {
    LOG(WARNING) << "Failed to choose a model to render.";
    return;
  }

  reconstruction_ = project_->GetStorage()->GetReconstruction(
      full_names[short_names.indexOf(QRegExp(reconstruction_name))]);

  if (!reconstruction_) {
    LOG(WARNING) << "Failed to get reconstruction to render!";
    return;
  }

  // Centers the reconstruction based on the absolute deviation of 3D points.
  reconstruction_->Normalize();
  world_points_.reserve(reconstruction_->NumTracks());

  for (const theia::TrackId track_id : reconstruction_->TrackIds()) {
    const auto* track = reconstruction_->Track(track_id);
    if (track == nullptr || !track->IsEstimated()) {
      continue;
    }

    QVector3D point_coords = QVector3D(
        track->Point().hnormalized().x(),
        track->Point().hnormalized().y(),
        track->Point().hnormalized().z());

    QColor point_color = QColor(
        static_cast<int>(track->Color()(0, 0)),
        static_cast<int>(track->Color()(1, 0)),
        static_cast<int>(track->Color()(2, 0)));

    WorldPoint world_point;
    world_point.coords = point_coords;
    world_point.color = point_color;
    world_point.trackId = track_id;
    world_point.is_found = false;
    world_point.is_selected = false;

    world_points_.emplace_back(world_point);
  }

  InitCameras();

  LOG(INFO) << "DRAWING: num world points: " << world_points_.size();
  LOG(INFO) << "DRAWING: num cameras: " << cameras_.size();

  draw();

  // To render the reconstruction without waiting to mouse click on widget.
  update();
}

void ReconstructionWindow::InitCameras() {
  cameras_.clear();

  cameras_.reserve(reconstruction_->NumViews());

  for (const theia::ViewId view_id : reconstruction_->ViewIds()) {
    const auto* view = reconstruction_->View(view_id);
    if (view == nullptr || !view->IsEstimated()) {
      continue;
    }
    ModifiedCamera camera(view->Camera());
    camera.SetViewName(QString::fromStdString(view->Name()));
    cameras_.emplace_back(camera);
  }

  UpdateHighlightedCameras();
}

void ReconstructionWindow::UpdateHighlightedCameras() {
  for (ModifiedCamera& camera : cameras_) {
    camera.SetHighlighted(highlighted_views_.contains(camera.GetViewName()));
  }

  if (!reconstruction_) { return; }

  std::multiset<theia::TrackId> selected_tracks;
  for (QString viewName : highlighted_views_) {
    const theia::View* view = reconstruction_->View(
        reconstruction_->ViewIdFromName(viewName.toStdString()));

    if (!view) { continue; }

    std::vector<theia::TrackId> tracks = view->TrackIds();
    std::copy(tracks.begin(), tracks.end(),
              std::inserter(selected_tracks, selected_tracks.end()));
  }

  SetSelectedPoints(selected_tracks);
}

void ReconstructionWindow::init() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glDisable(GL_LIGHTING);

  setSceneRadius(100.0);          // scene has a 100 OpenGL units radius
  camera()->showEntireScene();
}

void ReconstructionWindow::draw() {
  glBegin(GL_POINTS);

  for (WorldPoint point : world_points_) {
    auto coords = point.coords;
    auto color = point.color;

    if (point.is_found) {
      // Drawing founded track with Red.
      glColor3f(1.0, 0.0, 0.0);
    } else if (point.is_selected) {
      // Drawing track from selected view with Blue.
      glColor3f(0.0, 0.0, 1.0);
    } else {
      // Drawing track with it real color.
      glColor3f(color.redF(), color.greenF(), color.blueF());
    }
    glVertex3i(coords.x(), coords.y(), coords.z());
  }

  for (ModifiedCamera camera : cameras_) {
    DrawCamera(camera);
  }

  glEnd();
}

void ReconstructionWindow::DrawCamera(const ModifiedCamera& camera) {
  // Copied from view_reconstruction.cc
  // by Chris Sweeney (cmsweeney@cs.ucsb.edu)

  float normalized_focal_length = 1.0;

  glPushMatrix();
  Eigen::Matrix4d transformation_matrix = Eigen::Matrix4d::Zero();
  transformation_matrix.block<3, 3>(0, 0) =
      camera.GetOrientationAsRotationMatrix().transpose();
  transformation_matrix.col(3).head<3>() = camera.GetPosition();
  transformation_matrix(3, 3) = 1.0;

  // Apply world pose transformation.
  glMultMatrixd(reinterpret_cast<GLdouble*>(transformation_matrix.data()));

  // Draw Cameras.
  if (camera.IsHighlighted()) {
    // Drawing Highlighted cameras with Blue.
    glColor3f(0.0, 0.0, 1.0);
  } else {
    // The default color for cameras is Red.
    glColor3f(1.0, 0.0, 0.0);
  }

  // Create the camera wireframe. If intrinsic parameters are not set then use
  // the focal length as a guess.
  const float image_width =
      (camera.ImageWidth() == 0) ? camera.FocalLength()
                                 : camera.ImageWidth();
  const float image_height =
      (camera.ImageHeight() == 0) ? camera.FocalLength()
                                  : camera.ImageHeight();
  const float normalized_width = (image_width / 2.0) / camera.FocalLength();
  const float normalized_height = (image_height / 2.0) / camera.FocalLength();

  const Eigen::Vector3f top_left =
      normalized_focal_length *
      Eigen::Vector3f(-normalized_width, -normalized_height, 1);
  const Eigen::Vector3f top_right =
      normalized_focal_length *
      Eigen::Vector3f(normalized_width, -normalized_height, 1);
  const Eigen::Vector3f bottom_right =
      normalized_focal_length *
      Eigen::Vector3f(normalized_width, normalized_height, 1);
  const Eigen::Vector3f bottom_left =
      normalized_focal_length *
      Eigen::Vector3f(-normalized_width, normalized_height, 1);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(top_right[0], top_right[1], top_right[2]);
  glVertex3f(top_left[0], top_left[1], top_left[2]);
  glVertex3f(bottom_left[0], bottom_left[1], bottom_left[2]);
  glVertex3f(bottom_right[0], bottom_right[1], bottom_right[2]);
  glVertex3f(top_right[0], top_right[1], top_right[2]);
  glEnd();
  glPopMatrix();
}

void ReconstructionWindow::SetHighlightedViewNames(
    const QVector<QString>& views) {
  highlighted_views_ = views;
  UpdateHighlightedCameras();
  update();
}

void ReconstructionWindow::SetFoundPoints(
    const QSet<theia::TrackId>* found_tracks) {

  for (WorldPoint& point : world_points_) {
    point.is_found = found_tracks->contains(point.trackId);
  }

  update();
}

void ReconstructionWindow::SetSelectedPoints(
    const std::multiset<theia::TrackId> s_tracks) {

  for (WorldPoint& point : world_points_) {
    point.is_selected = s_tracks.find(point.trackId) != s_tracks.end();
  }
}

ReconstructionWindow::~ReconstructionWindow() {
  delete reconstruction_;
}
