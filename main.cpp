// Copyright 2017 Lybros.

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "cli/cli.h"
#include "ui/mainwindow.h"

#include <QApplication>

DEFINE_string(mode, "ui", "Application mode. Choose between ui and cli.");

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Logging is enabled.";

  if (FLAGS_mode == "ui") {
    LOG(INFO) << "Running UI.";
    QApplication a(argc, argv);
    QtAwesome* awesome = new QtAwesome(&a);
    awesome->initFontAwesome();

    MainWindow w;
    // Temporary avoiding rendering icons.
    // w.SetIcons(awesome);
    w.show();

    return a.exec();

  } else if (FLAGS_mode == "cli") {
    // Running in CLI mode means no need in using any code from ui/, as ui/ is
    // only a wrapper for a Project object.
    // In CLI mode we may simply create a Project and work with it directly.
    RunCLI();
    return 0;
  }

  LOG(ERROR) << "'mode' flag must be either 'ui' or 'cli'. "
                "If not provided, default value is 'ui'.";
  return 0;
}
