// Copyright 2017 Lybros.

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <QDir>
#include <QString>

QString FeatureFilenameFromImage(QString output_dir, QString image_path);

void EnsureTrailingSlash(QString* path);

QString FileNameFromPath(const QString& path);

#endif  // SRC_UTILS_H_
