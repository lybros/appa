// Copyright 2017 Lybros.

#include "storage.h"

Storage::Storage() {
  images_ = new QVector<QString>();
  images_path_ = "";
}

Storage::Storage(QString images_path) : images_path_(images_path) {
  Storage();
}

QString Storage::GetImagesPath() {
  return images_path_;
}

// TODO(uladbohdan): clean up out/ when dataset was changed
int Storage::UpdateImagesPath(QString images_path) {
  images_path_ = images_path;
  return ParseImageFolder();
}

bool Storage::ForceInitialize(QString images_path,
                              const QVector<QString>& images) {
  // Verifying if "images" are still in filesystem and accessible.
  for (const QString& image : images) {
    if (!QFileInfo(image).exists()) {
      LOG(ERROR) << "Image \"" << image.toStdString()
      << "\" not found";
      return false;
    }
  }

  // Everything seems to be fine. Force overriding the fields.
  images_path_ = images_path;
  images_ = new QVector<QString>(images);

  // This sort is redundant, as we're sorting images_ once and for all in
  // Storage::ParseImageFolder().
  // Anyway, we need to sort the images_ here too for projects which were
  // created before we added sorting during parsing.
  // TODO(uladbohdan): may be removed if no old-styled project configs exist.
  qSort(*images_);

  LOG(INFO) << "Force initialization: success. " << images_->length()
  << " read";

  return true;
}

int Storage::ParseImageFolder() {
  images_ = new QVector<QString>();
  QDirIterator it(images_path_, QDirIterator::Subdirectories);

  LOG(INFO) << "Reading images...";
  QRegExp rx(IMAGE_FILENAME_PATTERN);
  while (it.hasNext()) {
    QString next_image;
    next_image = it.next();

    if (rx.indexIn(next_image) == -1) {
      LOG(WARNING) << "\t" << next_image.toStdString() <<
      "- \"does not match the regex.\"";
      continue;
    }

    LOG(INFO) << "\t" << next_image.toStdString();
    images_->push_back(next_image);
  }

  // Sorting images once and for all.
  qSort(*images_);

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
