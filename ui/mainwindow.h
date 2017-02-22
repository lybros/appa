#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>

#include <QMainWindow>

#include "newproject.h"

#include "../src/project.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionRun_Reconstruction_triggered();
    void on_actionNewProject_triggered();

    void on_actionOpen_triggered();

private:
    Ui::MainWindow *ui;
    Project *active_project_;
};

#endif // MAINWINDOW_H
