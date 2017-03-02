// Created by Ivan Pazhitnykh on 02.03.17.

#include "utils.h"

QString FeatureFilenameFromImage(QString output_dir, QString image_path) {
    QString image_name =
            image_path.right(image_path.length() - 1 -
                             image_path.lastIndexOf("/")) + ".features";
    return QDir(output_dir).filePath("features/" + image_name);
}
