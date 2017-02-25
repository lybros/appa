// Copyright 2017 Lybros.

#include "storage.h"

Storage::Storage() {
    images_ = new QVector<QString>();
}

Storage::Storage(QString images_path) :
    images_path_(images_path) {
    Storage();
}

QString Storage::GetImagesPath() {
    return images_path_;
}

int Storage::UpdateImagesPath(QString images_path) {
    images_path_ = images_path;
    return ParseImageFolder();
}

bool Storage::ForceInitialize(QString images_path,
                              QVector<QString>& images) {
    // Verifying if "images" are still in filesystem and accessible.
    for (QString& image : images) {
        if (!QFileInfo(image).exists()) {
            std::cerr << "Image \"" << image.toStdString()
                      << "\" used to be in filesystem but not any more."
                      << std::endl;
            return false;
        }
    }

    // Everything seems to be fine. Force overriding the fields.
    images_path_ = images_path;
    images_ = new QVector<QString>(images);

    std::cout << "Force initialization: success. " << images_->length() << " read." << std::endl;

    return true;
}

int Storage::ParseImageFolder() {
    images_ = new QVector<QString>();
    QDirIterator it(images_path_, QDirIterator::Subdirectories);
    std::cout << "Reading images..." << std::endl;
    QRegExp rx(IMAGE_FILENAME_PATTERN);
    while (it.hasNext()) {
        QString next_image;
        next_image = it.next();
        if (rx.indexIn(next_image) != -1) {
            std::cout << next_image.toStdString() << std::endl;
            images_->push_back(next_image);
        } else {
            std::cout << "\"" << next_image.toStdString()
                      << "\" does not match the regex." << std::endl;
        }
    }
    return images_->length();
}

QVector<QString>& Storage::GetImages() {
    return *images_;
}

int Storage::NumberOfImages() {
    return images_->length();
}

Storage::~Storage() {
    if (images_) {
        delete images_;
    }
}
