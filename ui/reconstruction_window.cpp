// Copyright 2017 Lybros.

#include "reconstruction_window.h"

ReconstructionWindow::ReconstructionWindow(QWidget *parent) :
    QOpenGLWidget(parent) {
}

void ReconstructionWindow::BuildWithDefaultParameters(Project *project) {
}

void ReconstructionWindow::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void ReconstructionWindow::resizeGL(int w, int h) {
}

void ReconstructionWindow::paintGL() {
}

ReconstructionWindow::~ReconstructionWindow() {
}
