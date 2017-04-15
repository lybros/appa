// Copyright 2017 Lybros.

#ifndef UI_OPTIONS_DIALOG_H_
#define UI_OPTIONS_DIALOG_H_

#include <QComboBox>
#include <QDialog>
#include <QString>
#include <QTextStream>

#include "../src/options.h"

namespace Ui {
  class OptionsDialog;
}

const int GENERAL_OPTIONS             = 0b00000001;
const int EXTRACTING_FEATURES_OPTIONS = 0b00000010;
const int MATCHING_FEATURES_OPTIONS   = 0b00000100;
const int RECONSTRUCTION_OPTIONS      = 0b00001000;

class OptionsDialog : public QDialog {
 Q_OBJECT    // NOLINT(whitespace/indent)

 public:
  OptionsDialog(QWidget* parent, Options* options, int options_enabled);
  ~OptionsDialog();

  void InitializeForms();

  void accept() override;

 private:
  Ui::OptionsDialog *ui;

  const int options_enabled_;

  Options* options_;

  void EnableOptionSections();

  void FindSetCombobox(QComboBox* combobox, QString text);
};

#endif  // UI_OPTIONS_DIALOG_H_
