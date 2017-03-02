//
// Created by Ivan Pazhitnykh on 02.03.17.
//

#include "features.h"

// TODO(drapegnik): pass to constructor options object
Features::Features(QString out_path) : out_path_(out_path) { };

void Features::Extract(QVector<QString>& images) {
    return _extract(images, false);
}

void Features::ForceExtract(QVector<QString>& images) {
    return _extract(images, true);
};

void Features::_extract(QVector<QString>& images, bool is_force) {
    std::cout << "Start processing:" << std::endl;
    for (QString image_path : images) {
        QString feature_file = FeatureFilenameFromImage(out_path_, image_path);

        if (!QFileInfo::exists(feature_file) or is_force) {
            std::cout << "\t+ " << image_path.toStdString() << std::endl;
        }
    }
};

Features::~Features() {

};