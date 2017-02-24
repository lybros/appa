// Copyright 2017 Lybros.

#include "project.h"

Project::Project() {
    options_ = new ReconstructionBuilderOptions();
    options_->descriptor_type = theia::DescriptorExtractorType::SIFT;
    options_->output_matches_file = out_matches_file;
    options_->matching_options.match_out_of_core = true;
    options_->matching_options.perform_geometric_verification = false;
    options_->matching_options.keypoints_and_descriptors_output_dir =
            "out/matches";

    storage_ = new Storage();
}

Project::Project(QString project_name,
                 QString project_path,
                 QString images_path) :
    project_name_(project_name),
    project_path_(project_path) {
    Project();
    storage_->UpdateImagesPath(images_path);


    // Creating a Project in filesystem.
    // TODO(uladbohdan): to handle the situation when creating a folder fails.
    QDir project_parent_dir(project_path);
    if (!project_parent_dir.cdUp()) {
        std::cerr << "cdUp failed" << std::endl;
    }
    // std::cout << "PARENT PATH: " << project_parent_dir.currentPath().toStdString() << std::endl;
    if (project_parent_dir.mkdir(project_name)) {
        std::cout << "Project Directory seems to be created successfully!"
              << std::endl;
    } else {
        std::cout << "Creating Project Directory failed." << std::endl;
    }
    WriteConfigurationFile();
}

void Project::RunReconstruction() {
    const bool readMatchesFromFile = false;
    // google::InitGoogleLogging(argv[0]);
    ReconstructionBuilder reconstruction_builder(*options_);

    if (readMatchesFromFile) {
        vector<string> image_files;
        vector<theia::CameraIntrinsicsPrior> camera_intrinsics_prior;
        vector<theia::ImagePairMatch> image_matches;

        theia::ReadMatchesAndGeometry(
                out_matches_file,
                &image_files,
                &camera_intrinsics_prior,
                &image_matches);

        theia::CameraIntrinsicsGroupId intrinsics_group_id = 0;

        for (int i = 0; i < image_files.size(); i++) {
            reconstruction_builder.AddImageWithCameraIntrinsicsPrior(
                    image_files[i], camera_intrinsics_prior[i],
                        intrinsics_group_id);
        }


        for (const auto& match : image_matches) {
            cout << "(" << match.image1 << ", " << match.image2 << ")" << endl;
            CHECK(reconstruction_builder.AddTwoViewMatch(
                      match.image1, match.image2, match));
        }
    } else {
        for (int num = 1; num <= 9; num++) {
            reconstruction_builder.AddImage(theia::StringPrintf(
                                                "images/image00%d.jpg", num));
        }
        CHECK(reconstruction_builder.ExtractAndMatchFeatures())
        << "Could not extract and match features";
    }


    vector<Reconstruction*> reconstructions;
    CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
    << "Could not create a reconstruction";

    for (int i = 0; i < reconstructions.size(); i++) {
        string output_file = theia::StringPrintf("%s-%d.txt", "out/model", i);
        cout << "Writing reconstruction " << i << " to " <<
                output_file << endl;
        CHECK(theia::WriteReconstruction(*reconstructions[i], output_file))
        << "Could not write reconstruction to file";
    }
    return;
}

void Project::ExtractFeatures() {
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
        stream << "OUTPUT_LOCATION " << "DEFAULT" << endl;
    } else {
        return false;
    }
    configFile.close();
    return true;
}

bool Project::ReadConfigurationFile() {
    QFile configFile(GetConfigurationFilePath());

    if (configFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&configFile);
        QString temp_line;

        temp_line = stream.readLine();
        if (temp_line != "PROJECT_CONFIG_VERSION v1.0") {
            std::cerr << "Reading config failed: wrong file version."
                      << std::endl;
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
            std::cerr << "Wrong config file format. No IMAGES_LOCATION attribute."
                      << std::endl;
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
        if (temp_line == "DEFAULT") {
            output_location_ = GetDefaultOutputPath();
        } else {
            output_location_ = temp_line;
        }
    } else {
        return false;
    }
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
    // delete storage_;
}
