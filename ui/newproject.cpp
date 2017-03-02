// Copyright 2017 Lybros.

#include "newproject.h"
#include "ui_newproject.h"

NewProjectDialog::NewProjectDialog(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::NewProjectDialog) {
    ui->setupUi(this);
}

void NewProjectDialog::SetProjectOptions(NewProjectOptions* project_options) {
    project_options_ = project_options;
}

void NewProjectDialog::accept() {
    if (!create_new_project()) {
        LOG(ERROR) << "Failed to create new project";
        return;
    }

    LOG(INFO) << "Created successfully!";
    QDialog::accept();
}

bool NewProjectDialog::create_new_project() {
    if (verify_new_project_options()) {
        initialize_new_project();
        return true;
    } else {
        return false;
    }
}

bool NewProjectDialog::verify_new_project_options() {
    QString error = "";

    QString projectName = ui->project_name->toPlainText();
    if (projectName.isEmpty()) {
        error = "Project Name cannot be empty";
    }

    QString projectParentPath = ui->project_parent_path->text();
    if (projectParentPath.isEmpty()) {
        error = "Project Path cannot be empty";
    }
    if (!QFileInfo::exists(projectParentPath)) {
        error = "The path for project you've chosen does not exist!";
    }

    QDir projectParentDir(projectParentPath);

    if (QFileInfo::exists(projectParentDir.filePath(projectName))) {
        error = "You have to choose another name for your project! It's already exist.";
    }

    QString imagePath = ui->images_path->text();
    if (imagePath.isEmpty()) {
        error = "Images Path cannot be empty";
    }
    if (!QFileInfo::exists(imagePath)) {
        error = "Invalid path to images directory.";
    }

    if (!error.isEmpty()) {
        show_warning(error);
        return false;
    }
    return true;
}

bool NewProjectDialog::initialize_new_project() {
    project_options_->project_name = ui->project_name->toPlainText();

    QString project_parent_path = ui->project_parent_path->text();
    project_options_->project_path =
            QDir(project_parent_path).filePath(project_options_->project_name);

    project_options_->images_path = ui->images_path->text();
    LOG(INFO) << "Parameters to be returned back from NewProjectDialog:";
    std::cout << "\t" << project_options_->project_name.toStdString() <<
    std::endl;
    std::cout << "\t" << project_options_->project_path.toStdString() <<
    std::endl;
    std::cout << "\t" << project_options_->images_path.toStdString() <<
    std::endl;
    return true;
}

void NewProjectDialog::show_warning(QString& error) {
    QMessageBox warningBox;
    warningBox.setText(error);
    warningBox.exec();
}

NewProjectDialog::~NewProjectDialog() {
    delete ui;
}

void NewProjectDialog::on_locate_project_button_clicked() {
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Choose the directory"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    ui->project_parent_path->setText(dir);
}

void NewProjectDialog::on_locate_images_button_clicked() {
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Choose the directory"),
            QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    ui->images_path->setText(dir);
}
