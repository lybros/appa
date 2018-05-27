// Copyright 2017 Lybros.

#include "storage.h"

#include <QDateTime>

Storage::Storage() : options_(nullptr),
                     images_(nullptr),
                     images_path_(""),
                     slam_reduced_images_(nullptr),
                     output_location_(""),
                     loaded_reconstruction_(nullptr),
                     loaded_reconstruction_name_("") {
}

void Storage::SetOptions(Options* options) {
  options_ = options;
}

QString Storage::GetImagesPath() {
  return images_path_;
}

// TODO(uladbohdan): clean up out/ when dataset was changed.
int Storage::UpdateImagesPath(QString images_path) {
  EnsureTrailingSlash(&images_path);
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
  EnsureTrailingSlash(&images_path);
  images_path_ = images_path;
  images_ = new QVector<QString>(images);

  // This sort is redundant, as we're sorting images_ once and for all in
  // Storage::ParseImageFolder().
  // Anyway, we need to sort the images_ here too for projects which were
  // created before we added sorting during parsing.
  // TODO(uladbohdan): may be removed if no old-styled project configs exist.
  qSort(*images_);

  LOG(INFO) << "Force initialization: success " << images_->length() << " read";
  return true;
}

int Storage::ParseImageFolder() {
  delete images_;
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

Reconstruction* Storage::GetReconstruction(const QString reconstruction_name) {
  // Avoiding reloading model if it's already in memory.
  if (reconstruction_name != loaded_reconstruction_name_) {
    LoadReconstruction(reconstruction_name);
  }
  return loaded_reconstruction_;
}

void Storage::LoadModelsList() {
  reconstructions_.clear();

  QDirIterator it(QDir(output_location_).filePath("models/"));

  LOG(INFO) << "Loading list of models...";

  QRegExp rx(MODEL_FILENAME_PATTERN);
  rx.setPatternSyntax(QRegExp::Wildcard);
  while (it.hasNext()) {
    QString next_model = it.next();
    if (rx.exactMatch(next_model) == false) {
      LOG(WARNING) << "\t" << next_model.toStdString()
                   << "- \"does not match the regex.\"";
      continue;
    }

    LOG(INFO) << "\t Model found: " << next_model.toStdString();
    reconstructions_ << next_model;
  }

  LOG(INFO) << "Found " << reconstructions_.size() << " models";
}

void Storage::LoadReconstruction(QString reconstruction_name) {
  LOG(INFO) << "Loading model" << reconstruction_name.toStdString()
            << "to memory...";

  Reconstruction* reconstruction = new Reconstruction();
  CHECK(ReadReconstruction(reconstruction_name.toStdString(), reconstruction))
  << "Could not read model from file.";

  delete loaded_reconstruction_;

  loaded_reconstruction_ = reconstruction;
  loaded_reconstruction_name_ = reconstruction_name;

  LOG(INFO) << "Model " << loaded_reconstruction_name_.toStdString()
            << "was successfully loaded to memory.";
}

QStringList& Storage::GetReconstructions() {
  LoadModelsList();
  return reconstructions_;
}

const QString& Storage::GetOutputLocation() const {
  return output_location_;
}

bool Storage::SetOutputLocation(const QString& output_location) {
  // Checking if storage structure already exists and creating it, if it's not.
  if (!QFileInfo(output_location).exists()) {
    bool ok = QDir().mkpath(output_location);
    if (!ok) {
      return false;
    }
  }
  if (!QFileInfo(QDir(output_location).filePath("models")).exists()) {
    bool ok = QDir(output_location).mkdir("models");
    if (!ok) {
      return false;
    }
  }
  output_location_ = output_location;
  return true;
}

bool Storage::GetCalibration(
    QMap<QString, theia::CameraIntrinsicsPrior>* camera_intrinsics_prior,
    bool* shared_calibration) {
  return StorageIO(this).ReadCalibrationFile(GetCameraIntrinsicsPath(),
    camera_intrinsics_prior, shared_calibration);
}

QString Storage::GetCameraIntrinsicsPath() const {
  return QDir(images_path_).filePath(DEFAULT_CALIBRATION_FILE_NAME);
}

Storage::~Storage() {
  delete images_;
  delete slam_reduced_images_;
  delete loaded_reconstruction_;
}
