// Copyright 2017 Lybros.

/* project-config file is:
 * ----------------------------------------------------------------------------
 * PROJECT_CONFIG_VERSION v1.0
 * PROJECT_NAME "project-name"
 * IMAGES_LOCATION "~/datasets/dataset0/"
 * NUMBER_OF_IMAGES N_IMAGES
 * "name0.jpg"
 * ...
 * "nameN.jpg"
 * OUTPUT_LOCATION "~/project0/out"
 * ----------------------------------------------------------------------------
 * Notice: names of images should be sorted in ascending order. Images are
 * automatically sorted during the parsing, but you should keep that in mind if
 * you're creating a project config manually.
 *
 */

#ifndef SRC_IO_PROJECT_H_
#define SRC_IO_PROJECT_H_

class Project;

class ProjectIO {
 public:
  explicit ProjectIO(Project* project);

  bool WriteConfigurationFile();
  bool ReadConfigurationFile();

  ~ProjectIO();

 private:
  Project* project_;
};

#endif  // SRC_IO_PROJECT_H_
