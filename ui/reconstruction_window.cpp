// Copyright 2017 Lybros.

#include "reconstruction_window.h"

ReconstructionWindow::ReconstructionWindow() {
    QGLViewer();
}

ReconstructionWindow::ReconstructionWindow(Project *project) {
    ReconstructionWindow();
    project_.reset(project);
}

void ReconstructionWindow::BuildFromDefaultPath() {
    std::string filename =
            QDir(project_->GetOutputLocation()).filePath(DEFAULT_MODEL_BINARY_FILENAME).toStdString();
    // Output as a binary file.
    std::unique_ptr<theia::Reconstruction> reconstruction(
        new theia::Reconstruction());
    CHECK(ReadReconstruction(filename, reconstruction.get()))
        << "Could not read reconstruction file.";

    std::cout << "Read successfully from file." << std::endl;

    // Centers the reconstruction based on the absolute deviation of 3D points.
    reconstruction->Normalize();

    world_points_.reserve(reconstruction->NumTracks());

    std::cout << "num tracks: " << reconstruction->NumTracks() << std::endl;

    for (const theia::TrackId track_id : reconstruction->TrackIds()) {
        const auto* track = reconstruction->Track(track_id);
        if (track == nullptr || !track->IsEstimated()) {
            continue;
        }
        world_points_.emplace_back(QVector3D(
                    track->Point().hnormalized().x(),
                    track->Point().hnormalized().y(),
                    track->Point().hnormalized().z()
                                       ));
    }

    reconstruction.release();

    std::cout << "DRAWING: num world points: " <<
                 world_points_.size() << std::endl;
    draw();
}

void ReconstructionWindow::init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    setSceneRadius(100.0);          // scene has a 100 OpenGL units radius
    camera()->showEntireScene();
}

void ReconstructionWindow::draw() {
    glBegin(GL_POINTS);

    glColor3f(1.0f,0,0);
    // Red lines from (0,0,0) to understand the scale.
    for (float i = 0; i < 1000; i++) {
        glVertex3f(i,i,i);
        glVertex3f(-i,i,i);
        glVertex3f(i,-i,i);
        glVertex3f(-i,-i,i);
    }

    glColor3f(0,0,0);
    for (QVector3D point : world_points_) {
        glVertex3i(point.x(), point.y(), point.z());
    }

    glEnd();
}

ReconstructionWindow::~ReconstructionWindow() {
}
