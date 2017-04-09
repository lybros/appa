// Copyright 2017 Lybros.

#include "utils.h"

QString FeatureFilenameFromImage(QString output_dir, QString image_path) {
  QString image_name =
      image_path.right(image_path.length() - 1 -
                       image_path.lastIndexOf("/")) + ".features";
  return QDir(output_dir).filePath("features/" + image_name);
}

void EnsureTrailingSlash(QString* path) {
  if (path->toStdString()[path->length() - 1] != '/') {
    *path += "/";
  }
}

QString FileNameFromPath(const QString& path) {
  return path.mid(path.lastIndexOf("/", -2) + 1);
}

ReconstructionStatus StatusFromInt(int status) {
  switch (status) {
    case 0:
      return ReconstructionStatus::NOT_BUILT;
    case 1:
      return ReconstructionStatus::BUILT;
    case 2:
      return ReconstructionStatus::LOADED_INTO_MEMORY;
  }
}