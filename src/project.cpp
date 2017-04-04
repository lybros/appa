// Copyright 2017 Lybros.

#include "project.h"

Project::Project() : options_(new Options()), storage_(new Storage()) {
}

Project::Project(QString project_name,
                 QString project_path,
                 QString images_path) : project_name_(project_name),
                                        project_path_(project_path),
                                        options_(new Options()),
                                        storage_(new Storage()) {
  storage_->UpdateImagesPath(images_path);
  features_ = new Features(storage_, GetDefaultOutputPath());
  status_ = Options::ReconstructionStatus::NOT_BUILT;

  // Creating a Project in filesystem.
  // TODO(uladbohdan): to handle the situation when creating a folder fails.
  QDir project_parent_dir(project_path);

  if (!project_parent_dir.cdUp()) {
    LOG(ERROR) << "cdUp failed";
  }

  if (!project_parent_dir.mkdir(project_name)) {
    LOG(ERROR) << "Creating Project Directory failed.";
  }

  LOG(INFO) << "Project Directory seems to be created successfully!";
  WriteConfigurationFile();

  // Creating out/ directory.
  QDir(project_path).mkdir("out");
  output_location_ = GetDefaultOutputPath();

  options_ = new Options(output_location_);
}

// Simple build from scratch and save into a binary file.
// Covers all stages all together:
// * Extracting features, saving them into a filesystem.
// * Matching features.
// * Building a 3D model and saving it into a binary file in filesystem.
//
// May take plenty of time to finish processing.
//
// The recommendation is to run every stage separately.
void Project::BuildModelToBinary() {
  ReconstructionBuilderOptions* options =
      options_->GetReconstructionBuilderOptions();

  ReconstructionBuilder reconstruction_builder(*options);

  for (QString image_path : storage_->GetImages()) {
    reconstruction_builder.AddImage(image_path.toStdString());
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

  reconstructions_.resize(reconstructions.size());
  for (int i = 0; i < reconstructions_.size(); i++) {
    reconstructions_[i].reset(reconstructions[i]);
  }
  status_ = Options::ReconstructionStatus::LOADED_INTO_MEMORY;

  std::string output_file_template =
      QDir(output_location_).filePath("model").toStdString();

  for (int i = 0; i < reconstructions.size(); i++) {
    std::string output_file =
        theia::StringPrintf("%s-%d.binary", output_file_template.c_str(), i);
    LOG(INFO) << "Writing reconstruction " << i << " to " << output_file;
    CHECK(theia::WriteReconstruction(*reconstructions[i], output_file))
    << "Could not write reconstruction to file";
  }

  LOG(INFO) << "Reconstruction has been saved to filesystem.";
  return;
}

void Project::SearchImage(QString file_path,
                          QString model_name,
                          QSet<theia::TrackId>* h_tracks) {
  std::string model_path = QDir(output_location_)
      .filePath(model_name).toStdString();

  Reconstruction model;
  CHECK(theia::ReadReconstruction(model_path, &model))
  << "Could not read reconstruction from file: " << model_path;
  LOG(INFO) << "Successfully read model " << model_name.toStdString();

  std::vector<theia::Keypoint> keypoints;
  std::vector<Eigen::VectorXf> descriptors;

  features_->ExtractFeature(file_path, &keypoints, &descriptors);
  LOG(INFO) << "Successfully extract " << descriptors.size() << " features";

  std::unordered_map<std::string, Features::FeaturesMap> feature_to_descriptor;
  features_->GetFeaturesMap(&feature_to_descriptor);

  theia::L2 norm;
  std::vector<theia::TrackId> tracks_ids = model.TrackIds();
  int counter = 0;
  theia::Timer timer;

  for (auto t_id : tracks_ids) {
    theia::Track* track = model.MutableTrack(t_id);
    std::unordered_set<theia::ViewId> views_ids = track->ViewIds();
    bool match = false;

    for (auto v_id : views_ids) {
      theia::View* view = model.MutableView(v_id);
      const theia::Feature feature = *(view->GetFeature(t_id));
      const Features::Pair key = std::make_pair(feature[0], feature[1]);
      Features::FeaturesMap::const_iterator got =
          feature_to_descriptor[view->Name()].find(key);

      if (got == feature_to_descriptor[view->Name()].end()) {
        LOG(INFO) << "Feature not found [" << view->Name()
                  << "]: (" << feature[0] << ", " << feature[1] << ")";
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
  LOG(INFO) << "Match " << counter << "/" << model.NumTracks() << " tracks";
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

  if (configFile.open(QIODevice::ReadWrite)) {
    QTextStream stream(&configFile);
    stream << "PROJECT_CONFIG_VERSION v1.1" << endl;
    stream << "PROJECT_NAME " << project_name_ << endl;
    stream << "STATUS " << status_ << endl;
    stream << "IMAGES_LOCATION " << GetImagesPath() << endl;
    stream << "NUMBER_OF_IMAGES " << storage_->NumberOfImages() << endl;
    for (auto image_path : storage_->GetImages()) {
      stream << image_path << endl;
    }
    stream << "OUTPUT_LOCATION " << GetDefaultOutputPath() << endl;
  } else {
    return false;
  }
  configFile.close();
  return true;
}

bool Project::ReadConfigurationFile() {
  QFile configFile(GetConfigurationFilePath());

  if (!configFile.open(QIODevice::ReadOnly)) {
    return false;
  }

  QTextStream stream(&configFile);
  QString temp_line;

  temp_line = stream.readLine();
  if (temp_line != "PROJECT_CONFIG_VERSION v1.1") {
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
  if (temp_line != "STATUS") {
    LOG(ERROR) << "Wrong config file format. No STATUS attribute.";
    configFile.close();
    return false;
  }

  int status;
  stream >> status;
  status_ = StatusFromConfigFile(status);

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
  output_location_ = temp_line;
  features_ = new Features(storage_, output_location_);

  configFile.close();
  return true;
}

QString Project::GetConfigurationFilePath() {
  return QDir(project_path_).filePath(CONFIG_FILE_NAME);
}

QString Project::GetDefaultOutputPath() {
  return QDir(project_path_).filePath(DEFAULT_OUTPUT_LOCATION_POSTFIX);
}

QString Project::GetOutputLocation() {
  return output_location_;
}

std::vector<std::shared_ptr<theia::Reconstruction>>&
Project::GetReconstructions() {
  return reconstructions_;
}

Storage* Project::GetStorage() {
  return storage_;
}

Project::~Project() {
  WriteConfigurationFile();
  delete options_;
  delete storage_;
  delete features_;
}
