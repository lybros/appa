#include "newproject.h"
#include "ui_newproject.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
}

void NewProjectDialog::SetActiveProject(Project *active_project)
{
    project = active_project;
}

void NewProjectDialog::accept()
{
    QDialog::accept();
    if (verify_new_project_options()) {
        initialize_new_project();
    } else {
        show_warning();
    }
}

bool NewProjectDialog::verify_new_project_options()
{
    return true;
}

bool NewProjectDialog::initialize_new_project()
{
    return true;
}

void NewProjectDialog::show_warning()
{
    return;
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}
