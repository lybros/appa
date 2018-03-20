// Copyright 2017 Lybros.

#include "projectio.h"

#include "../project.h"

#include <QDir>
#include <QFile>
#include <QTextStream>

ProjectIO::ProjectIO(Project *project) : project_(project), pr(project) {
}

ProjectIO::~ProjectIO() {
}

bool ProjectIO::WriteConfigurationFile() {
  QFile configFile(pr->GetConfigurationFilePath());
  if (!configFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
    return false;
  }

  QTextStream stream(&configFile);
  stream << "PROJECT_CONFIG_VERSION v1.1" << endl;
  stream << "PROJECT_NAME " << pr->project_name_ << endl;
  stream << "IMAGES_LOCATION " << pr->GetImagesPath() << endl;
  stream << "NUMBER_OF_IMAGES " << pr->storage_->NumberOfImages() << endl;
  int images_location_path_length = pr->GetImagesPath().length();
  for (QString image_path : pr->storage_->GetImages()) {
    stream << image_path.right(image_path.length() -
                               images_location_path_length) << endl;
  }
  stream << "OUTPUT_LOCATION " << pr->GetDefaultOutputPath() << endl;

  configFile.close();
  return true;
}

bool ProjectIO::ReadConfigurationFile() {
  QFile configFile(pr->GetConfigurationFilePath());

  if (!configFile.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&configFile);
  QString temp_line;

  temp_line = stream.readLine();

  bool read_status;

  if (temp_line == "PROJECT_CONFIG_VERSION v1.0") {
    read_status = ReadConfigurationFileV1_0(stream);
  } else if (temp_line == "PROJECT_CONFIG_VERSION v1.1") {
    read_status = ReadConfigurationFileV1_1(stream);
  } else {
    LOG(ERROR) << "Reading config failed: wrong file version.";
    read_status = false;
  }

  configFile.close();

  return read_status;
}

bool ProjectIO::ReadConfigurationFileV1_0(QTextStream& stream) {
  return ReadProjectName(stream) &&
         ReadImagesV1_0(stream) &&
         ReadOutputLocation(stream);
}

bool ProjectIO::ReadConfigurationFileV1_1(QTextStream& stream) {
  return ReadProjectName(stream) &&
         ReadImagesV1_1(stream) &&
         ReadOutputLocation(stream);
}

bool ProjectIO::ReadProjectName(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "PROJECT_NAME") {
    LOG(ERROR) << "Wrong config file format. No PROJECT_NAME attribute.";
    return false;
  }
  stream >> pr->project_name_;
  return true;
}

bool ProjectIO::ReadImagesV1_0(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    return false;
  }
  int number_of_images;
  stream >> number_of_images;
  QVector<QString> images(number_of_images);
  for (int i = 0; i < number_of_images; i++) {
    stream >> images[i];
  }

  if (!pr->storage_->ForceInitialize(images_path, images)) {
    LOG(ERROR) << "Force storage initializing failed :(";
    return false;
  }

  return true;
}

bool ProjectIO::ReadImagesV1_1(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    return false;
  }
  int number_of_images;
  stream >> number_of_images;
  QVector<QString> images(number_of_images);
  for (int i = 0; i < number_of_images; i++) {
    stream >> temp_line;
    images[i] = QDir(images_path).filePath(temp_line);
  }

  if (!pr->storage_->ForceInitialize(images_path, images)) {
    LOG(ERROR) << "Force storage initializing failed :(";
    return false;
  }

  return true;
}

bool ProjectIO::ReadOutputLocation(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "OUTPUT_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No OUTPUT_LOCATION attr.";
    return false;
  }
  stream >> temp_line;
  CHECK(pr->storage_->SetOutputLocation(temp_line))
      << "Failed to initialize output location from config file.";
  return true;
}

bool ProjectIO::ReadSlamData() {
  return true;
}
