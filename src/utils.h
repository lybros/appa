// Created by Ivan Pazhitnykh on 02.03.17.

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <QString>
#include <QDir>

QString FeatureFilenameFromImage(QString output_dir, QString image_path);

void EnsureTrailingSlash(QString& path);

#endif  // SRC_UTILS_H_
