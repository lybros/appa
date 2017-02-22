#include "project.h"

Project::Project()
{
    options_ = new ReconstructionBuilderOptions();
    options_->descriptor_type = theia::DescriptorExtractorType::SIFT;
    options_->output_matches_file = out_matches_file;
    options_->matching_options.match_out_of_core = true;
    options_->matching_options.perform_geometric_verification = false;
    options_->matching_options.keypoints_and_descriptors_output_dir = "out/matches";

    storage_ = new Storage();
}

void Project::RunReconstruction()
{
    const bool readMatchesFromFile = false;
    //google::InitGoogleLogging(argv[0]);
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
                    image_files[i], camera_intrinsics_prior[i], intrinsics_group_id);
        }


        for (const auto& match : image_matches) {
            cout << "(" << match.image1 << ", " << match.image2 << ")" << endl;
            CHECK(reconstruction_builder.AddTwoViewMatch(match.image1, match.image2, match));
        }
    } else {
        for (int num = 1; num <= 9; num++) {
            reconstruction_builder.AddImage(theia::StringPrintf("images/image00%d.jpg", num));
        }
        CHECK(reconstruction_builder.ExtractAndMatchFeatures())
        << "Could not extract and match features";
    }


    vector<Reconstruction*> reconstructions;
    CHECK(reconstruction_builder.BuildReconstruction(&reconstructions))
    << "Could not create a reconstruction";

    for (int i = 0; i < reconstructions.size(); i++) {
        string output_file = theia::StringPrintf("%s-%d.txt", "out/model", i);
        cout << "Writing reconstruction " << i << " to " << output_file << endl;
        CHECK(theia::WriteReconstruction(*reconstructions[i], output_file))
        << "Could not write reconstruction to file";
    }
    return;
}

QString Project::GetProjectName()
{
    return project_name_;
}

QString Project::GetProjectPath()
{
    return project_path_;
}

QString Project::GetImagesPath()
{
    return images_path_;
}

void Project::SetProjectName(QString project_name)
{
    project_name_ = project_name;
}

void Project::SetProjectPath(QString project_path)
{
    project_path_ = project_path;
}

void Project::SetImagesPath(QString images_path)
{
    images_path_ = images_path;
    storage_->UpdateImagesPath(images_path);
}

bool Project::WriteConfigurationFile()
{
    QFile configFile(GetConfigurationFilePath());
    if (configFile.open(QIODevice::ReadWrite)) {
        QTextStream stream(&configFile);
        stream << "PROJECT_CONFIG_VERSION v1.0" << endl;
        stream << "PROJECT_NAME " << project_name_ << endl;
        stream << "IMAGES_LOCATION " << images_path_ << endl;
        stream << "NUMBER_OF_IMAGES " << 0 << endl;
        // TODO(uladbohdan): to write list of images to the configuration file.
        // TODO(uladbohdan): to figure out if the output location is a part
        // of project configutation.
        stream << "OUTPUT_LOCATION " << "default" << endl;
    } else {
        return false;
    }
    configFile.close();
    return true;
}

QString Project::GetConfigurationFilePath()
{
    QString projectFolder = QDir(project_path_).filePath(project_name_);
    QString configFilePath = QDir(projectFolder).filePath("project-config");
    return configFilePath;
}

Project::~Project()
{
    delete options_;
}
