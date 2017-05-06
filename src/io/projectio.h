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
 * project-config v1.1 description:
 * upd:
 *  - each image is described with relative to IMAGES_LOCATION path.
 *    That means full image is IMAGES_LOCATION/"name0.jpg".
 *
 * Notice: keep in mind, that reconstruction (colorizing) fails for datasets
 * which are organized in non-plane structure (e.g. folder contains several
 * folders with images). The STRONG recommendation is to use datasets with the
 * following structure:
 * images_path/
 * |--image0.jpg
 * ...
 * |--imageN.jpg
 */

#ifndef SRC_IO_PROJECTIO_H_
#define SRC_IO_PROJECTIO_H_

#include <QTextStream>

class Project;

class ProjectIO {
 public:
  explicit ProjectIO(Project* project);

  bool WriteConfigurationFile();
  bool ReadConfigurationFile();

  ~ProjectIO();

 private:
  Project* project_;
  Project* pr;  // pr is a shortcut for the same Project.

  // Supporting all versions of configuration files.
  bool ReadConfigurationFileV1_0(QTextStream& stream);
  bool ReadConfigurationFileV1_1(QTextStream& stream);

  // To avoid code repeat from different versions of config files.
  bool ReadProjectName(QTextStream& stream);
  bool ReadImagesV1_0(QTextStream& stream);
  bool ReadImagesV1_1(QTextStream& stream);
  bool ReadOutputLocation(QTextStream& stream);
  bool ReadOptionsV1_1(QTextStream& stream);
};

#endif  // SRC_IO_PROJECTIO_H_
