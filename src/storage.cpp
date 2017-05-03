// Copyright 2017 Lybros.

#include "storage.h"

Storage::Storage() : options_(nullptr),
                     images_(nullptr),
                     images_path_(""),
                     output_location_(""),
                     status_(ReconstructionStatus::NOT_BUILT) {
}

void Storage::SetOptions(Options* options) {
  options_ = options;
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

// TODO(all): maybe some refactor to get model by name instead index.
Reconstruction* Storage::GetReconstruction(int number) {
  if (status_ != ReconstructionStatus::LOADED_INTO_MEMORY) {
    ReadReconstructions();
  }
  if (reconstructions_.size() < number + 1) { return NULL; }

  return reconstructions_[number];
}

void Storage::SetReconstructions(
    const std::vector<Reconstruction*>& reconstructions) {
  // Removing old reconstructions.
  for (auto reconstruction : reconstructions_) {
    delete reconstruction;
  }

  reconstructions_.resize(reconstructions.size());
  for (int i = 0; i < reconstructions_.size(); i++) {
    reconstructions_[i] = reconstructions[i];
  }

  if (!reconstructions_.size()) {
    status_ = ReconstructionStatus::NOT_BUILT;
    return;
  }

  status_ = ReconstructionStatus::LOADED_INTO_MEMORY;
  LOG(INFO) << "Reconstructions have been saved to memory.";
}

void Storage::ReadReconstructions() {
  QDirIterator it(output_location_);
  std::vector<Reconstruction*> reconstructions;

  LOG(INFO) << "Reading models...";
  QRegExp rx(MODEL_FILENAME_PATTERN);
  while (it.hasNext()) {
    QString next_model;
    next_model = it.next();

    if (rx.indexIn(next_model) == -1) {
      LOG(WARNING) << "\t" << next_model.toStdString() <<
                   "- \"does not match the regex.\"";
      continue;
    }

    LOG(INFO) << "\t" << next_model.toStdString();
    QString filename = QDir(output_location_).filePath(next_model);

    Reconstruction* reconstruction = new Reconstruction();
    CHECK(ReadReconstruction(filename.toStdString(), reconstruction))
    << "Could not read model from file.";

    reconstructions.push_back(reconstruction);
  }

  SetReconstructions(reconstructions);
  LOG(INFO) << "Successfully load " << reconstructions_.size() << " models";
}

void Storage::WriteReconstructions() {
  std::string output_file_template =
      QDir(output_location_).filePath("model").toStdString();

  for (int i = 0; i < reconstructions_.size(); i++) {
    std::string output_file =
        theia::StringPrintf("%s-%d.binary", output_file_template.c_str(), i);
    LOG(INFO) << "Writing reconstruction " << i << " to " << output_file;
    CHECK(theia::WriteReconstruction(*reconstructions_[i], output_file))
    << "Could not write reconstruction to file";
  }

  LOG(INFO) << "Reconstructions has been saved to filesystem.";
}

void Storage::SetReconstructionStatus(ReconstructionStatus status) {
  status_ = status;
}

ReconstructionStatus Storage::GetReconstructionStatus() const {
  return status_;
}

const QString& Storage::GetOutputLocation() const {
  return output_location_;
}

void Storage::SetOutputLocation(const QString& output_location) {
  output_location_ = output_location;
}

bool Storage::GetCalibration(
    QMap<QString, theia::CameraIntrinsicsPrior>* camera_intrinsics_prior) {
  return StorageIO(this).ReadCalibrationFile(GetCameraIntrinsicsPath(),
                                             camera_intrinsics_prior);
}

QString Storage::GetCameraIntrinsicsPath() const {
  return QDir(images_path_).filePath(DEFAULT_CALIBRATION_FILE_NAME);
}

Storage::~Storage() {
  delete images_;
  for (auto reconstruction : reconstructions_) {
    delete reconstruction;
  }
}
