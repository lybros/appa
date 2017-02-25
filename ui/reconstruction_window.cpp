// Copyright 2017 Lybros.

#include "reconstruction_window.h"

ReconstructionWindow::ReconstructionWindow() {
    scene_ = new Qt3DCore::QEntity;

    // Material
    material = new Qt3DExtras::QPhongMaterial(scene_);

    // Torus
    torusEntity = new Qt3DCore::QEntity(scene_);
    torusMesh = new Qt3DExtras::QTorusMesh;
    torusMesh->setRadius(5);
    torusMesh->setMinorRadius(1);
    torusMesh->setRings(100);
    torusMesh->setSlices(20);

    torusTransform = new Qt3DCore::QTransform;
    torusTransform->setScale3D(QVector3D(1.5, 1, 0.5));
    torusTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));

    torusEntity->addComponent(torusMesh);
    torusEntity->addComponent(torusTransform);
    torusEntity->addComponent(material);

    // Camera
    cameraEntity = camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    setRootEntity(scene_);
}

ReconstructionWindow::~ReconstructionWindow() {
    delete scene_;

    delete material;
    delete torusEntity;
    delete torusMesh;
    delete torusTransform;
}
