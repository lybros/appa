// Copyright 2017 Lybros.

#include "cli.h"

#include <QDateTime>
#include <QDir>
#include <QString>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "../src/project.h"

/* The minimum sets of parameters in CLI mode (to pass verification) are:
 *   new + project_name + project_path + images_path + action
 *   open + action
 *   temp + images_path + action
 */

DEFINE_string(cli_mode, "",
              "In CLI mode you may either create a new project "
              "or open an existent one. Choose between open, new or temp "
              "(temp means new + removal after processing).");
DEFINE_string(project_path, "",
              "If you're in 'open' cli_mode, you must specify the project.");
DEFINE_string(project_name, "",
              "You must provide a project name in order to create a new one.");
DEFINE_string(images_path, "",
              "You must provide a dataset in order to create a new project.");

DEFINE_string(action, "",
              "Choose if you want to extract features, match them or run "
              "reconstruction. Visualization is only available in 'ui' mode");

bool VerifyCommandlineArguments() {
  if (FLAGS_cli_mode != "new" &&
      FLAGS_cli_mode != "open" &&
      FLAGS_cli_mode != "temp") {
    LOG(ERROR) << "Wrong cli_mode value.";
    return false;
  }

  // TODO(uladbohdan): to support more actions.
  if (FLAGS_action != "reconstruction") {
    LOG(ERROR) << "You must pass a valid action to proceed.";
    return false;
  }

  if (FLAGS_cli_mode == "new") {
    if (FLAGS_project_name == "" ||
        FLAGS_project_path == "" ||
        FLAGS_images_path == "") {
      LOG(ERROR) << "You must provide project_name, project_path and "
                    "images_path to create a new project.";
      return false;
    }
  }

  if (FLAGS_cli_mode == "open") {
    if (FLAGS_project_path == "") {
      LOG(ERROR) << "You must provide project_path to open a project.";
      return false;
    }
  }

  if (FLAGS_cli_mode == "temp") {
    if (FLAGS_images_path == "") {
      LOG(ERROR) << "You must provide images_path to create a temp project";
      return false;
    }
  }

  return true;
}

void RunCLI() {
  LOG(INFO) << "Running in CLI mode.";

  if (!VerifyCommandlineArguments()) {
    LOG(ERROR) << "CL arguments verification failed.";
    return;
  }

  Project* project;

  QString temp_project_path;

  if (FLAGS_cli_mode == "open") {
    project = new Project(QString::fromStdString(FLAGS_project_path));
  } else if (FLAGS_cli_mode == "new") {
    project = new Project(QString::fromStdString(FLAGS_project_name),
                          QString::fromStdString(FLAGS_project_path),
                          QString::fromStdString(FLAGS_images_path));
  } else if (FLAGS_cli_mode == "temp") {
    QString temp_project_name =
        QString("appa-project-") +
        QDateTime::currentDateTime().toString(Qt::ISODate);
    temp_project_path = QDir::temp().filePath(temp_project_name);
    LOG(INFO) << "Temp project will be created in "
              << temp_project_path.toStdString();
    project = new Project(temp_project_name,
                          temp_project_path,
                          QString::fromStdString(FLAGS_images_path));
  }

  if (FLAGS_action == "reconstruction") {
    LOG(INFO) << "Starting reconstruction...";
    project->BuildModelToBinary();
  }

  if (FLAGS_cli_mode == "temp") {
    if (QDir(temp_project_path).removeRecursively()) {
      LOG(INFO) << "Temp project was successfully removed.";
    } else {
      LOG(ERROR) << "Failed to remove a temporary project.";
    }
  }

  LOG(INFO) << "Quitting the application...";
  delete project;
}
