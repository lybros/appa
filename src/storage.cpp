#include "storage.h"

Storage::Storage()
{

}

Storage::Storage(QString images_path) :
    images_path_(images_path)
{
    Storage();
}

QString Storage::GetImagesPath()
{
    return images_path_;
}

int Storage::UpdateImagesPath(QString images_path)
{
    images_path_ = images_path;
    return ParseImageFolder();
}

bool Storage::ForceInitialize(QString images_path, QVector<QString>& images)
{
    return true;
}

int Storage::ParseImageFolder()
{
    return 0;
}

QVector<QString>& Storage::GetImages()
{
    return images_;
}

int Storage::NumberOfImages()
{
    return images_.length();
}

Storage::~Storage()
{

}
