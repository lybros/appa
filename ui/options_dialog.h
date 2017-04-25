// Copyright 2017 Lybros.

#ifndef UI_OPTIONS_DIALOG_H_
#define UI_OPTIONS_DIALOG_H_

#include <QComboBox>
#include <QCheckBox>
#include <QDialog>
#include <QString>
#include <QTextStream>

#include "../src/options.h"

namespace Ui { class OptionsDialog; }

using theia::OptimizeIntrinsicsType;

const int GENERAL_OPTIONS             = 0b00000001;
const int EXTRACTING_FEATURES_OPTIONS = 0b00000010;
const int MATCHING_FEATURES_OPTIONS   = 0b00000100;
const int RECONSTRUCTION_OPTIONS      = 0b00001000;

/* HOWTO add a new option to the dialog.
 * 1. The option must be in Options class (flag, string, number or whatever).
 * 2. To add the label and the data-input widget to the options_dialog.ui file
 *    to the appropriate section. To give the widget non-default name.
 * 3. To implement reading a default (or read from config file) value in
 *    OptionsDialog::InitializeForms() method.
 * 4. To set an updated value back to options_ in OptionsDialog::accept()
 *    method. Additional verification may be needed.
 * ****************************************************************************/

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

  // Helpers to reduce the code size.
  void FindSetCombobox(QComboBox* combobox, QString text);
  void SetCheckbox(QCheckBox* checkbox, OptimizeIntrinsicsType opt_type);
};

#endif  // UI_OPTIONS_DIALOG_H_
