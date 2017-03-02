//
// Created by Ivan Pazhitnykh on 02.03.17.
//

#include <iostream>
#include "utils.h"

QString FeatureFilenameFromImage(QString output_dir, QString image) {
    return output_dir
           + "features/"
           + image.right(image.length() - 1 - image.lastIndexOf("/"))
           + ".features";
}