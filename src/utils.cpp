// Copyright 2017 Lybros.

#include "utils.h"

QString FeatureFilenameFromImage(QString output_dir, QString image_path) {
    QString image_name =
            image_path.right(image_path.length() - 1 -
                             image_path.lastIndexOf("/")) + ".features";
    return QDir(output_dir).filePath("features/" + image_name);
}

void EnsureTrailingSlash(QString& path) {
    if (path[path.length()-1] != "/") {
        path += "/";
    }
}
