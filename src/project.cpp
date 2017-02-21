#include "project.h"

Project::Project()
{
    options = new ReconstructionBuilderOptions();
    options->descriptor_type = theia::DescriptorExtractorType::SIFT;
    options->output_matches_file = out_matches_file;
    options->matching_options.match_out_of_core = true;
    options->matching_options.perform_geometric_verification = false;
    options->matching_options.keypoints_and_descriptors_output_dir = "out/matches";
}

void Project::RunReconstruction()
{
    const bool readMatchesFromFile = false;
    //google::InitGoogleLogging(argv[0]);
    ReconstructionBuilder reconstruction_builder(*options);

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

QString Project::getProjectName()
{
    return project_name;
}

QString Project::getProjectPath()
{
    return project_path;
}

QString Project::getImagesPath()
{
    return images_path;
}

void Project::setProjectName(QString _project_name)
{
    project_name = _project_name;
}

void Project::setProjectPath(QString _project_path)
{
    project_path = _project_path;
}

void Project::setImagesPath(QString _images_path)
{
    images_path = _images_path;
}

bool Project::WriteConfigurationFile()
{
    QFile configFile(getConfigurationFilePath());
    if (configFile.open(QIODevice::ReadWrite)) {
        QTextStream stream(&configFile);
        stream << "PROJECT_CONFIG_VERSION v1.0" << endl;
    } else {
        return false;
    }
    configFile.close();
    return true;
}

QString Project::getConfigurationFilePath()
{
    QString projectFolder = QDir(project_path).filePath(project_name);
    QString configFilePath = QDir(projectFolder).filePath("project-config");
    return configFilePath;
}

Project::~Project()
{
    delete options;
}
