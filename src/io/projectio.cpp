// Copyright 2017 Lybros.

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "projectio.h"

#include "../project.h"

#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QVector>

#include <Eigen/Geometry>

#include <theia/sfm/camera/camera.h>

ProjectIO::ProjectIO(Project *project) : project_(project), pr(project) {
}

ProjectIO::~ProjectIO() {
}

bool ProjectIO::WriteConfigurationFile() {
  QFile configFile(pr->GetConfigurationFilePath());
  if (!configFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
    return false;
  }

  QTextStream stream(&configFile);
  stream << "PROJECT_CONFIG_VERSION v1.1" << endl;
  stream << "PROJECT_NAME " << pr->project_name_ << endl;
  stream << "IMAGES_LOCATION " << pr->GetImagesPath() << endl;
  stream << "NUMBER_OF_IMAGES " << pr->storage_->NumberOfImages() << endl;
  int images_location_path_length = pr->GetImagesPath().length();
  for (QString image_path : pr->storage_->GetImages()) {
    stream << image_path.right(image_path.length() -
                               images_location_path_length) << endl;
  }
  stream << "OUTPUT_LOCATION " << pr->GetDefaultOutputPath() << endl;

  configFile.close();
  return true;
}

bool ProjectIO::ReadConfigurationFile() {
  QFile configFile(pr->GetConfigurationFilePath());

  if (!configFile.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&configFile);
  QString temp_line;

  temp_line = stream.readLine();

  bool read_status;

  if (temp_line == "PROJECT_CONFIG_VERSION v1.0") {
    read_status = ReadConfigurationFileV1_0(stream);
  } else if (temp_line == "PROJECT_CONFIG_VERSION v1.1") {
    read_status = ReadConfigurationFileV1_1(stream);
  } else {
    LOG(ERROR) << "Reading config failed: wrong file version.";
    read_status = false;
  }

  configFile.close();

  return read_status;
}

bool ProjectIO::ReadConfigurationFileV1_0(QTextStream& stream) {
  return ReadProjectName(stream) &&
         ReadImagesV1_0(stream) &&
         ReadOutputLocation(stream);
}

bool ProjectIO::ReadConfigurationFileV1_1(QTextStream& stream) {
  return ReadProjectName(stream) &&
         ReadImagesV1_1(stream) &&
         ReadOutputLocation(stream);
}

bool ProjectIO::ReadProjectName(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "PROJECT_NAME") {
    LOG(ERROR) << "Wrong config file format. No PROJECT_NAME attribute.";
    return false;
  }
  stream >> pr->project_name_;
  return true;
}

bool ProjectIO::ReadImagesV1_0(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    return false;
  }
  int number_of_images;
  stream >> number_of_images;
  QVector<QString> images(number_of_images);
  for (int i = 0; i < number_of_images; i++) {
    stream >> images[i];
  }

  if (!pr->storage_->ForceInitialize(images_path, images)) {
    LOG(ERROR) << "Force storage initializing failed :(";
    return false;
  }

  return true;
}

bool ProjectIO::ReadImagesV1_1(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    return false;
  }
  int number_of_images;
  stream >> number_of_images;
  QVector<QString> images(number_of_images);
  for (int i = 0; i < number_of_images; i++) {
    stream >> temp_line;
    images[i] = QDir(images_path).filePath(temp_line);
  }

  if (!pr->storage_->ForceInitialize(images_path, images)) {
    LOG(ERROR) << "Force storage initializing failed :(";
    return false;
  }

  return true;
}

bool ProjectIO::ReadOutputLocation(QTextStream& stream) {
  QString temp_line;
  stream >> temp_line;
  if (temp_line != "OUTPUT_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No OUTPUT_LOCATION attr.";
    return false;
  }
  stream >> temp_line;
  CHECK(pr->storage_->SetOutputLocation(temp_line))
      << "Failed to initialize output location from config file.";
  return true;
}

bool ProjectIO::ReadCameras() {
  QFile trajectoryFile("/home/parallels/co-graph/KeyFrameTrajectory.txt");

  // TUM format: The format of each line is 'timestamp tx ty tz qx qy qz qw'

  LOG(INFO) << "Now we are reading KeyFrameTrajectory file...";

  if (!trajectoryFile.open(QIODevice::ReadOnly)) { return false; }

  QVector<QString>& imageNames = pr->GetStorage()->GetImages();

  QTextStream stream(&trajectoryFile);
  while (!stream.atEnd()) {
    double ts;
    stream >> ts; // the timestamp.
    double tx, ty, tz, qx, qy, qz, qw;
    stream >> tx >> ty >> tz >> qx >> qy >> qz >> qw;

    // tx ty tz are world camera position. Might be enought to perform the
    // experiment.

    theia::Camera* camera = new theia::Camera();
    camera->SetFocalLength(517.);
    camera->SetPrincipalPoint(318.643040, 255.313989);
    camera->SetImageSize(640, 480);

    camera->SetPosition(Eigen::Vector3d(tx, ty, tz));
//    LOG(INFO) << "Camera: " << camera.GetPosition() << "  PUSHED.";
    auto q = Eigen::Quaterniond{qx, qy, qz, qw};
    auto rotMatrix = q.normalized().toRotationMatrix();
    LOG(INFO) << rotMatrix;
    camera->SetOrientationFromRotationMatrix(rotMatrix);

    pr->slam_cameras_.push_back(*camera);
    if (ts > 0.0001) {
//      LOG(INFO) << "TS " << ts << " segfault?";
      QString image_name = imageNames[(int)ts - 1];
      pr->slam_camera_names_.push_back(image_name);
      std::string short_name = FileNameFromPath(image_name).toStdString();
      pr->slam_cameras_map_[short_name] = *camera;
      LOG(INFO) << "OK so for image " << short_name << " camera POS is "
                << pr->slam_cameras_map_[short_name].GetPosition();
    }
  }

  // to avoid adding fake (0,0,0) camera in the end.
  pr->slam_cameras_.pop_back();

  LOG(INFO) << "SLAM CAMERAS PUSHED: " << pr->slam_cameras_.size();
}

bool ProjectIO::ReadSlamData() {
  ReadCameras();

  QFile slamFile("/home/parallels/co-graph/co-graph.txt");

  LOG(INFO) << "in ProjectIO class.";

  if (!slamFile.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&slamFile);
  QString temp_line;

  int number_of_keyframes;

  stream >> temp_line; // NUMBER_OF_KEYFRAMES
  stream >> number_of_keyframes;

  LOG(INFO) << "Number of keyframes read: " << number_of_keyframes;

  std::vector<int> kfIDs;

  // maps ID of a world point to list of KF IDs where it is observed from.
  std::unordered_map<int, std::vector<int>> observations;

  // this maps keyframeID : worldPointID : 2d_coordinates
  std::unordered_map<int, std::unordered_map<int, std::pair<int,int>>> image_coordinates;

  QVector<QString>& imageNames = pr->GetStorage()->GetImages();

  for (int i = 0; i < number_of_keyframes; i++) {
    stream >> temp_line; // KF
    int kfId;
    stream >> kfId;
    stream >> temp_line; // COVISIBLE_WITH
    int covisibleWith;
    stream >> covisibleWith;
    for (int j = 0; j < covisibleWith; j++) {
      stream >> temp_line;
      // Seems like I do not need this information at the moment.
    }
    stream >> temp_line; // NUMBER_OF_POINTS
    int number_points;
    stream >> number_points;
//    LOG(INFO) << "number of points: " << number_points;
    for (int j = 0; j < number_points; j++) {
      int pointId, x, y;
      stream >> pointId >> x >> y;
//      LOG(INFO) << "about to update maps...";
      image_coordinates[kfId][pointId] = std::make_pair(x, y);
      observations[kfId].push_back(pointId);
//      LOG(INFO) << "done.";

      pr->slam_observations_[pointId].push_back(
            Observation(pointId,
                        FileNameFromPath(imageNames[kfId - 1]).toStdString(),
                        x, y)
          );
    }
    kfIDs.push_back(kfId);
  }

  for (auto it = observations.begin(); it != observations.end(); it++) {
    std::sort(it->second.begin(), it->second.end());
  }

  std::sort(kfIDs.begin(), kfIDs.end());

  LOG(INFO) << "all sorted.";

  int LARGE_CONST = 10000;
  int MIN_SIZE_THRESHOLD = 10;

  for (int i = 0; i < kfIDs.size(); i++) {
    for (int j = i+1; j < kfIDs.size(); j++) {
      int kf1 = kfIDs[i];
//      LOG(INFO) << "First point vector: kf1=" << kf1;
//      LOG(INFO) << "Size: " << observations[kf1].size();
//      for (auto it = observations[kf1].begin(); it != observations[kf1].end(); it++) {
//        LOG(INFO) << *it;
//      }
      int kf2 = kfIDs[j];
      int pairID = kf1 * LARGE_CONST + kf2;
      std::vector<int> merged(2000);
      std::vector<int>::iterator it = std::set_intersection(
            observations[kf1].begin(), observations[kf1].end(),
            observations[kf2].begin(), observations[kf2].end(), merged.begin());
      merged.resize(it-merged.begin());

//      LOG(INFO) << kf1 << ":" << kf2 << " merged.";

      if (merged.size() < MIN_SIZE_THRESHOLD) {
//        LOG(INFO) << kf1 << ":" << kf2 << " MERGED SIZE TOO SMALL " << merged.size();
        continue;
      }

      theia::ImagePairMatch pMatch;

      for (int k = 0; k < merged.size(); k++) {
//        LOG(INFO) << "about to push...";
        pMatch.correspondences.push_back(
              theia::FeatureCorrespondence(
                theia::Feature(image_coordinates[kf1][merged[k]].first,
                               image_coordinates[kf1][merged[k]].second),
                theia::Feature(image_coordinates[kf2][merged[k]].first,
                               image_coordinates[kf2][merged[k]].second))
            );
//        LOG(INFO) << "done.";
      }
//      LOG(INFO) << "???";

      pMatch.image1 = imageNames[kf1 - 1].toStdString();
      pMatch.image2 = imageNames[kf2 - 1].toStdString();

      if (pMatch.image1.length() > 1 && pMatch.image2.length() > 1) {
        pr->slam_data_[pairID] = pMatch;
//        LOG(INFO) << "INFO PUSHED FOR " << kf1 << ":" << kf2
//                  << "  which are " << pMatch.image1 << " " << pMatch.image2
//                  << "  matches: " << merged.size() << endl;
       } else {
        LOG(INFO) << " ####  ALERT BROKEN NAMES";
      }
    }
  }

  slamFile.close();

  return true;
}
