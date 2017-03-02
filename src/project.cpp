// Copyright 2017 Lybros.

#include "project.h"

Project::Project() {
    options_ = new ReconstructionBuilderOptions();
    storage_ = new Storage();
}

Project::Project(QString project_name,
                 QString project_path,
                 QString images_path) : project_name_(project_name),
                                        project_path_(project_path) {
    Project();
    storage_->UpdateImagesPath(images_path);
    features_ = new Features(storage_, GetDefaultOutputPath());

    // Creating a Project in filesystem.
    // TODO(uladbohdan): to handle the situation when creating a folder fails.
    QDir project_parent_dir(project_path);

    if (!project_parent_dir.cdUp()) {
        LOG(ERROR) << "cdUp failed" << std::endl;
    }

    if (!project_parent_dir.mkdir(project_name)) {
        LOG(ERROR) << "Creating Project Directory failed." << std::endl;
    }

    LOG(INFO) << "Project Directory seems to be created successfully!" <<
    std::endl;
    WriteConfigurationFile();

    // Creating out/ directory.
    QDir(project_path).mkdir("out");
}

// Simple build from scratch and save into a binary file.
void Project::BuildModelToBinary() {
    options_->descriptor_type = theia::DescriptorExtractorType::SIFT;
    options_->output_matches_file =
            QDir(output_location_).filePath("matches.binary").toStdString();
    options_->matching_options.match_out_of_core = true;
    options_->matching_options.perform_geometric_verification = false;
    options_->matching_options.keypoints_and_descriptors_output_dir =
            QDir(output_location_).filePath("features").toStdString();

    ReconstructionBuilder reconstruction_builder(*options_);

    for (QString image_path : storage_->GetImages()) {
        reconstruction_builder.AddImage(image_path.toStdString());
    }
    CHECK(reconstruction_builder.ExtractAndMatchFeatures())
    << "Could not extract and match features";

    std::vector<Reconstruction*> reconstructions;
    CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
    << "Could not create a reconstruction";

    std::string output_file_template =
            QDir(output_location_).filePath("model").toStdString();

    for (int i = 0; i < reconstructions.size(); i++) {
        std::string output_file =
                theia::StringPrintf("%s-%d.binary",
                                    output_file_template.c_str(), i);
        LOG(INFO) << "Writing reconstruction " << i << " to "
        << output_file;
        CHECK(theia::WriteReconstruction(*reconstructions[i], output_file))
        << "Could not write reconstruction to file";
    }
    return;
}

void Project::SearchImage(QString filename) {
    std::cout << "Start work with image: " << filename.toStdString() <<
    std::endl;
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
        stream << "PROJECT_CONFIG_VERSION v1.0" << endl;
        stream << "PROJECT_NAME " << project_name_ << endl;
        stream << "IMAGES_LOCATION " << GetImagesPath() << endl;
        stream << "NUMBER_OF_IMAGES " << storage_->NumberOfImages() << endl;
        for (auto image_path : storage_->GetImages()) {
            stream << image_path << endl;
        }
        stream << "OUTPUT_LOCATION "
        << GetDefaultOutputPath() << endl;
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
    if (temp_line != "PROJECT_CONFIG_VERSION v1.0") {
        std::cerr << "Reading config failed: wrong file version." << std::endl;
        configFile.close();
        return false;
    }

    stream >> temp_line;
    if (temp_line != "PROJECT_NAME") {
        std::cerr << "Wrong config file format. No PROJECT_NAME attribute."
        << std::endl;
        configFile.close();
        return false;
    }

    stream >> project_name_;

    stream >> temp_line;
    if (temp_line != "IMAGES_LOCATION") {
        std::cerr <<
        "Wrong config file format. No IMAGES_LOCATION attribute." << std::endl;
        configFile.close();
        return false;
    }

    QString images_path;
    stream >> images_path;

    stream >> temp_line;
    if (temp_line != "NUMBER_OF_IMAGES") {
        std::cerr << "Wrong config file format. No NUMBER_OF_IMAGES attr."
        << std::endl;
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
        std::cerr << "Force storage initializing failed :(" << std::endl;
        configFile.close();
        return false;
    }

    stream >> temp_line;
    if (temp_line != "OUTPUT_LOCATION") {
        std::cerr << "Wrong config file format. No OUTPUT_LOCATION attr."
        << std::endl;
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

Project::~Project() {
    delete options_;
    delete storage_;
    delete features_;
}
