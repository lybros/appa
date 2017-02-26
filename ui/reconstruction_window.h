// Copyright 2017 Lybros.

#ifndef UI_RECONSTRUCTION_WINDOW_H_
#define UI_RECONSTRUCTION_WINDOW_H_

#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <vector>

#include <Eigen/Core>
#include <theia/theia.h>

#include "../src/project.h"

class ReconstructionWindow : public Qt3DExtras::Qt3DWindow {
 public:
    ReconstructionWindow();

    void BuildWithDefaultParameters(Project *project);

    ~ReconstructionWindow();

 private:
    Qt3DCore::QEntity *scene_;

    Qt3DCore::QEntity *torusEntity;
    Qt3DExtras::QTorusMesh *torusMesh;
    Qt3DCore::QTransform *torusTransform;
    Qt3DRender::QCamera *cameraEntity;
    Qt3DRender::QMaterial *material;

    Qt3DCore::QEntity *torus2;
    Qt3DExtras::QTorusMesh *torus2Mesh;
};

#endif  // UI_RECONSTRUCTION_WINDOW_H_
