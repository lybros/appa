// Copyright 2017 Lybros.

#include "reconstruction_window.h"

ReconstructionWindow::ReconstructionWindow() {
    QGLViewer();
}

ReconstructionWindow::ReconstructionWindow(Project *project) {
    ReconstructionWindow();
    project_ = project;
}

void ReconstructionWindow::UpdateActiveProject(Project *project) {
    project_ = project;
}

void ReconstructionWindow::BuildFromDefaultPath() {
    world_points_.clear();
    cameras_.clear();

    std::string filename =
            QDir(project_->GetOutputLocation()).filePath(
                DEFAULT_MODEL_BINARY_FILENAME).toStdString();
    // Output as a binary file.
    std::unique_ptr<theia::Reconstruction> reconstruction(
        new theia::Reconstruction());
    CHECK(ReadReconstruction(filename, reconstruction.get()))
        << "Could not read reconstruction file.";

    LOG(INFO) << "Read successfully from file.";

    // Colorizing the reconstruction using the raw image data.
    theia::ColorizeReconstruction(project_->GetImagesPath().toStdString(), 1,
                                  reconstruction.get());

    LOG(INFO) << "Reconstruction colorized successfully!";

    // Centers the reconstruction based on the absolute deviation of 3D points.
    reconstruction->Normalize();

    world_points_.reserve(reconstruction->NumTracks());

    std::cout << "num tracks: " << reconstruction->NumTracks() << std::endl;

    for (const theia::TrackId track_id : reconstruction->TrackIds()) {
        const auto* track = reconstruction->Track(track_id);
        if (track == nullptr || !track->IsEstimated()) {
            continue;
        }

        QVector3D point_coords = QVector3D(
                    track->Point().hnormalized().x(),
                    track->Point().hnormalized().y(),
                    track->Point().hnormalized().z());

        QColor point_color = QColor(
                    (int)track->Color()(0,0),
                    (int)track->Color()(1,0),
                    (int)track->Color()(2,0));

        WorldPoint world_point;
        world_point.coords = point_coords;
        world_point.color = point_color;

        world_points_.emplace_back(world_point);
    }

    cameras_.reserve(reconstruction->NumViews());
    for (const theia::ViewId view_id : reconstruction->ViewIds()) {
      const auto* view = reconstruction->View(view_id);
      if (view == nullptr || !view->IsEstimated()) {
        continue;
      }
      cameras_.emplace_back(view->Camera());
    }

    reconstruction.release();

    std::cout << "DRAWING: num world points: " <<
                 world_points_.size() << std::endl;
    std::cout << "DRAWING: num cameras: " << cameras_.size() << std::endl;
    draw();
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

        glColor3f(color.redF(), color.greenF(), color.blueF());
        glVertex3i(coords.x(), coords.y(), coords.z());
    }

    for (theia::Camera camera : cameras_) {
        DrawCamera(camera);
    }

    glEnd();
}

void ReconstructionWindow::DrawCamera(const theia::Camera& camera) {
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
    glColor3f(1.0, 0.0, 0.0);

    // Create the camera wireframe. If intrinsic parameters are not set then use
    // the focal length as a guess.
    const float image_width =
        (camera.ImageWidth() == 0) ? camera.FocalLength() : camera.ImageWidth();
    const float image_height =
        (camera.ImageHeight() == 0) ? camera.FocalLength() : camera.ImageHeight();
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

ReconstructionWindow::~ReconstructionWindow() {
}
