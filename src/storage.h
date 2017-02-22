#ifndef STORAGE_H
#define STORAGE_H

#include <QVector>

class Storage {

public:
    Storage();

    Storage(QString images_path);

    // Returns the number of images parsed from new source.
    int UpdateImagesPath(QString images_path);

    // Returns the number of images parsed.
    int ParseImageFolder();

    int NumberOfImages();

    QVector<QString>& GetImages();

    ~Storage();

private:
    QVector<QString> images_;
    QString images_path_;

};

#endif // STORAGE_H
