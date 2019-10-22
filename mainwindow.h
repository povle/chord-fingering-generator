#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_spinBox_valueChanged(const QString &arg1);

    void on_generateButton_clicked();

    void on_prevButton_clicked();

    void on_nextButton_clicked();

    void on_tuningEdit_textEdited(const QString &arg1);

    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    int currentPage;
    int strings;
    bool validateTuning();
    std::vector<QString> charts;
};
#endif // MAINWINDOW_H
