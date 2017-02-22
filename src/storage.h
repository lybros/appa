#ifndef STORAGE_H
#define STORAGE_H

#include <QVector>

class Storage {

public:
    Storage();

    Storage(QString images_path);

    QString GetImagesPath();

    // Returns the number of images parsed from new source.
    int UpdateImagesPath(QString images_path);

    // In case we load the project - we want to be sure we're only adding
    // the images we've had before. This method allows to avoid parsing and
    // put only the images we pass.
    // Verification than will check if the images are still exist.
    bool ForceInitialize(QString images_path, QVector<QString>& images);

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
