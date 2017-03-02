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

class Features {
public:
    Features(QString out_path);

    void Extract(QVector<QString>& images);

    void ForceExtract(QVector<QString>& images);

    void Match();

    void ForceMatch();

    ~Features();

protected:
    void _extract(QVector<QString>& images, bool is_force);

    void _match();

private:
    QString out_path_;
};

#endif //CW_CODE_FEATURES_H
