//
// Created by Ivan Pazhitnykh on 02.03.17.
//

#ifndef CW_CODE_FEATURES_H
#define CW_CODE_FEATURES_H

#include <iostream>
#include <theia/theia.h>

#include <QString>
#include <QVector>
#include <QFileInfo>

#include "utils.h"
#include "storage.h"

class Features {
public:
    Features(Storage* storage, QString out_path);

    // extract features only if doesn't find *.features files
    void Extract();

    // rewrite all *.features files
    void ForceExtract();

    void Match();

    void ForceMatch();

    ~Features();

protected:
    void _extract(bool is_force);

    void _match();

private:
    QString out_path_;
    QVector<QString> images_;
    Storage* storage_;
    theia::FeatureExtractor::Options options_;
    theia::FeatureExtractor* extractor_;
};

#endif //CW_CODE_FEATURES_H
