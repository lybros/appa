// Copyright 2017 Lybros.

#include "project.h"

Project::Project(QString project_path) :
    project_path_(project_path), storage_(new Storage()) {
  // This constructor assumes we're opening an existent project.
  // TODO(uladbohdan): to throw exceptions in case we're failed.

  CHECK(ReadConfigurationFile()) << "Reading config file failed!";

  options_ = new Options(storage_->GetOutputLocation());
  features_ = new Features(storage_, options_);
}

Project::Project(QString project_name,
                 QString project_path,
                 QString images_path) : project_name_(project_name),
                                        project_path_(project_path),
                                        storage_(new Storage()) {
  storage_->UpdateImagesPath(images_path);

  // Creating a Project in filesystem.
  // TODO(uladbohdan): to handle the situation when creating a folder fails.
  QDir project_parent_dir(project_path);

  CHECK(project_parent_dir.cdUp()) << "cdUp failed";

  CHECK(project_parent_dir.mkdir(project_name))
  << "Creating Project Directory failed.";

  LOG(INFO) << "Project Directory was created successfully!";

  WriteConfigurationFile();

  // Creating out/ directory.
  QDir(project_path).mkdir("out");
  storage_->SetOutputLocation(GetDefaultOutputPath());
  options_ = new Options(storage_->GetOutputLocation());
  features_ = new Features(storage_, options_);
}

void Project::BuildModelToBinary() {
  ReconstructionBuilderOptions options =
      options_->GetReconstructionBuilderOptions();

  ReconstructionBuilder reconstruction_builder(options);

  theia::CameraIntrinsicsGroupId intrinsics_group_id =
      theia::kInvalidCameraIntrinsicsGroupId;
  if (options_->CameraCalibrationIsShared()) {
    intrinsics_group_id = 0;
  }

  for (QString image_path : storage_->GetImages()) {
    reconstruction_builder.AddImage(image_path.toStdString(),
                                    intrinsics_group_id);
  }
  LOG(INFO) << "All images are added to the builder.";
  LOG(INFO) << "Starting extracting and matching";

  CHECK(reconstruction_builder.ExtractAndMatchFeatures())
  << "Could not extract and match features";

  LOG(INFO) << "Extracted and matched successfully!";

  std::vector<theia::Reconstruction*> reconstructions;
  CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
  << "Could not create a reconstruction";

  LOG(INFO) << "Reconstruction has been created.";

  // Using raw image data to colorize the reconstructions.
  // The method is running using 2 threads.
  for (int i = 0; i < reconstructions.size(); i++) {
    theia::ColorizeReconstruction(GetImagesPath().toStdString(), 2,
                                  reconstructions[i]);
  }
  LOG(INFO) << "Reconstruction colorized successfully!";

  storage_->SetReconstructions(reconstructions);
  storage_->WriteReconstructions();
  return;
}

void Project::SearchImage(QString file_path,
                          QSet<theia::TrackId>* h_tracks) {
  // TODO(drapegnik): replace hardcode index with some value.
  Reconstruction* model = storage_->GetReconstruction(0);
  if (!model) {
    LOG(WARNING) << "There is no built models!";
    return;
  }

  std::vector<theia::Keypoint> keypoints;
  std::vector<Eigen::VectorXf> descriptors;

  features_->ExtractFeature(file_path, &keypoints, &descriptors);
  LOG(INFO) << "Successfully extract " << descriptors.size() << " features";

  std::unordered_map<std::string, Features::FeaturesMap> feature_to_descriptor;
  features_->GetFeaturesMap(&feature_to_descriptor);

  theia::L2 norm;
  std::vector<theia::TrackId> tracks_ids = model->TrackIds();
  int counter = 0;
  theia::Timer timer;

  for (auto t_id : tracks_ids) {
    theia::Track* track = model->MutableTrack(t_id);
    std::unordered_set<theia::ViewId> views_ids = track->ViewIds();
    bool match = false;

    for (auto v_id : views_ids) {
      theia::View* view = model->MutableView(v_id);
      const theia::Feature feature = *(view->GetFeature(t_id));
      const Features::Pair key = std::make_pair(feature[0], feature[1]);
      Features::FeaturesMap::const_iterator got =
          feature_to_descriptor[view->Name()].find(key);

      if (got == feature_to_descriptor[view->Name()].end()) {
        LOG(INFO) << "Feature not found [" << view->Name() << "]: ("
                  << feature[0] << ", " << feature[1] << ")";
        continue;
      }

      Eigen::VectorXf track_descriptor = got->second;
      for (auto feature_descriptor : descriptors) {
        float distance = norm(track_descriptor, feature_descriptor);
        if (distance <= 0.1) {
          match = true;
          counter++;
          break;
        }
      }

      if (match) { break; }
    }

    if (match) { h_tracks->insert(t_id); }
  }
  const double time = timer.ElapsedTimeInSeconds();
  LOG(INFO) << "It took " << time << " seconds to compare descriptors";
  LOG(INFO) << "Match " << counter << "/" << model->NumTracks() << " tracks";
  return;
}

void Project::ExtractFeatures() {
  features_->ForceExtract();
}

void Project::MatchFeatures() {
}

void Project::StartReconstruction() {
}

QString Project::GetProjectName() {
  return project_name_;
}

QString Project::GetProjectPath() {
  return project_path_;
}

QString Project::GetImagesPath() {
  return storage_->GetImagesPath();
}

void Project::SetProjectName(QString project_name) {
  project_name_ = project_name;
}

void Project::SetProjectPath(QString project_path) {
  project_path_ = project_path;
}

void Project::SetImagesPath(QString images_path) {
  storage_->UpdateImagesPath(images_path);
}

bool Project::WriteConfigurationFile() {
  QFile configFile(GetConfigurationFilePath());
  if (!configFile.open(QIODevice::ReadWrite)) { return false; }

  QTextStream stream(&configFile);
  stream << "PROJECT_CONFIG_VERSION v1.0" << endl;
  stream << "PROJECT_NAME " << project_name_ << endl;
  stream << "IMAGES_LOCATION " << GetImagesPath() << endl;
  stream << "NUMBER_OF_IMAGES " << storage_->NumberOfImages() << endl;
  for (auto image_path : storage_->GetImages()) {
    stream << image_path << endl;
  }
  stream << "OUTPUT_LOCATION " << GetDefaultOutputPath() << endl;

  configFile.close();
  return true;
}

bool Project::ReadConfigurationFile() {
  QFile configFile(GetConfigurationFilePath());

  if (!configFile.open(QIODevice::ReadOnly)) { return false; }

  QTextStream stream(&configFile);
  QString temp_line;

  temp_line = stream.readLine();
  if (temp_line != "PROJECT_CONFIG_VERSION v1.0") {
    LOG(ERROR) << "Reading config failed: wrong file version.";
    configFile.close();
    return false;
  }

  stream >> temp_line;
  if (temp_line != "PROJECT_NAME") {
    LOG(ERROR) << "Wrong config file format. No PROJECT_NAME attribute.";
    configFile.close();
    return false;
  }
  stream >> project_name_;

  stream >> temp_line;
  if (temp_line != "IMAGES_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No IMAGES_LOCATION attribute.";
    configFile.close();
    return false;
  }
  QString images_path;
  stream >> images_path;

  stream >> temp_line;
  if (temp_line != "NUMBER_OF_IMAGES") {
    LOG(ERROR) << "Wrong config file format. No NUMBER_OF_IMAGES attr.";
    configFile.close();
    return false;
  }
  int number_of_images;
  stream >> number_of_images;
  QVector<QString> images(number_of_images);
  for (int i = 0; i < number_of_images; i++) {
    stream >> images[i];
  }

  if (!storage_->ForceInitialize(images_path, images)) {
    LOG(ERROR) << "Force storage initializing failed :(";
    configFile.close();
    return false;
  }

  stream >> temp_line;
  if (temp_line != "OUTPUT_LOCATION") {
    LOG(ERROR) << "Wrong config file format. No OUTPUT_LOCATION attr.";
    configFile.close();
    return false;
  }
  stream >> temp_line;
  storage_->SetOutputLocation(temp_line);

  configFile.close();
  return true;
}

QString Project::GetConfigurationFilePath() {
  return QDir(project_path_).filePath(CONFIG_FILE_NAME);
}

QString Project::GetDefaultOutputPath() {
  return QDir(project_path_).filePath(DEFAULT_OUTPUT_LOCATION_POSTFIX);
}


Storage* Project::GetStorage() {
  return storage_;
}

Options* Project::GetOptions() {
  return options_;
}

Project::~Project() {
  WriteConfigurationFile();
  delete options_;
  delete storage_;
  delete features_;
}
