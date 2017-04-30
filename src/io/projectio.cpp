// Copyright 2017 Lybros.

#include "projectio.h"

#include "../project.h"

#include <QFile>
#include <QTextStream>

ProjectIO::ProjectIO(Project *project) : project_(project) {
}

ProjectIO::~ProjectIO() {
}

bool ProjectIO::WriteConfigurationFile() {
  Project* pr = project_;

  QFile configFile(pr->GetConfigurationFilePath());
  if (!configFile.open(QIODevice::ReadWrite)) { return false; }

  QTextStream stream(&configFile);
  stream << "PROJECT_CONFIG_VERSION v1.0" << endl;
  stream << "PROJECT_NAME " << pr->project_name_ << endl;
  stream << "IMAGES_LOCATION " << pr->GetImagesPath() << endl;
  stream << "NUMBER_OF_IMAGES " << pr->storage_->NumberOfImages() << endl;
  for (auto image_path : pr->storage_->GetImages()) {
    stream << image_path << endl;
  }
  stream << "OUTPUT_LOCATION " << pr->GetDefaultOutputPath() << endl;

  configFile.close();
  return true;
}

bool ProjectIO::ReadConfigurationFile() {
  Project* pr = project_;

  QFile configFile(pr->GetConfigurationFilePath());

  if (!configFile.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&configFile);
  QString temp_line;

  temp_line = stream.readLine();
  if (temp_line != "PROJECT_CONFIG_VERSION v1.0") {
    LOG(ERROR) << "Reading config failed: wrong file version.";
    configFile.close();
    return false;
  }

  stream >> temp_line;
  if (temp_line != "PROJECT_NAME") {
    LOG(ERROR) << "Wrong config file format. No PROJECT_NAME attribute.";
    configFile.close();
    return false;
  }
  stream >> pr->project_name_;

  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    configFile.close();
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    configFile.close();
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
    configFile.close();
    return false;
  }

  stream >> temp_line;
  if (temp_line != "OUTPUT_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No OUTPUT_LOCATION attr.";
    configFile.close();
    return false;
  }
  stream >> temp_line;
  pr->storage_->SetOutputLocation(temp_line);

  configFile.close();
  return true;
}
