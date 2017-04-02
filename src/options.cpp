// Copyright 2017 Lybros.

#include "options.h"

Options::Options() {
}

Options::Options(QString output_location) :
        output_location_(output_location) {
}

theia::ReconstructionBuilderOptions*
Options::GetReconstructionBuilderOptions() {
    // TODO(uladbohdan): to figure out, where a 'delete' operator should be
    // located if we have a 'new' operator here.
    theia::ReconstructionBuilderOptions* options =
            new theia::ReconstructionBuilderOptions();

    options->descriptor_type = descriptor_type_;
    options->output_matches_file =
            QDir(output_location_).filePath("matches.binary").toStdString();
    options->matching_options.match_out_of_core = match_out_of_core_;
    options->matching_options.perform_geometric_verification =
            perform_geometric_verification_;
    options->matching_options.keypoints_and_descriptors_output_dir =
            QDir(output_location_).filePath("features").toStdString();

    return options;
}

Options::~Options() {
}
