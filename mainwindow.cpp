#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chord.h"
#include <QMessageBox>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    strings = 6;
    QRegularExpression re("([A-G][b#]?){6}");
    QValidator *validator = new QRegularExpressionValidator(re, this);
    ui->tuningEdit->setValidator(validator);
    currentPage = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::validateTuning(){
    int pos = 0;
    QString txt = ui->tuningEdit->text();
    bool valid = ui->tuningEdit->validator()->validate(txt, pos) == QValidator::State::Acceptable;
    ui->generateButton->setEnabled(valid);
    return valid;
}

void MainWindow::on_spinBox_valueChanged(const QString &arg1)
{
    QString txt = ui->tuningEdit->text();
    int pos = 0;
    int val = arg1.toInt();

    if(ui->tuningEdit->validator()->validate(txt, pos) == QValidator::State::Acceptable && val < this->strings){
        int toChop = 1;
        if (txt.back() == 'b' || txt.back() == '#'){
            toChop++;
        }
        ui->tuningEdit->setText(txt.chopped(toChop));
        ui->comboBox->setCurrentText(QString("Custom"));
    }

    delete ui->tuningEdit->validator();
    QRegularExpression re("([A-G][b#]?){"+arg1+"}");
    QValidator *validator = new QRegularExpressionValidator(re, this);
    ui->tuningEdit->setValidator(validator);

    validateTuning();

    this->strings = val;
}


void MainWindow::on_generateButton_clicked()
{
    std::string root = ui->chordRootBox->currentText().split('/')[0].toStdString();
    Chord chord(root, ui->chordQualityBox->currentText().toStdString());
    std::vector<int> tuning;
    QString tuningtext = ui->tuningEdit->text();
    QRegularExpression re("[A-G][b#]?");
    QRegularExpressionMatchIterator i = re.globalMatch(tuningtext);
    while (i.hasNext()){
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()){
            tuning.push_back(Chord::noteToVal(match.captured(0).toStdString()));
        }
    }
    try{
        this->charts = chord.getCharts(tuning);
        this->currentPage = 0;
        ui->nextButton->setDisabled(this->charts.empty());
        ui->prevButton->setDisabled(true);
        if (!this->charts.empty()){
            ui->chartBrowser->setText(this->charts[this->currentPage]);
        }
    }
    catch(const std::length_error&){
        QMessageBox::critical(this, QString("Error"), QString("This chord can't be played on selected number of strings"));
    }

}

void MainWindow::on_prevButton_clicked()
{
    this->currentPage--;
    if (this->currentPage==0){
        ui->prevButton->setDisabled(true);
    }
    if (this->currentPage < this->charts.size()-1){
        ui->nextButton->setEnabled(true);
    }
    ui->chartBrowser->setText(this->charts[this->currentPage]);
}

void MainWindow::on_nextButton_clicked()
{
    if (this->currentPage < this->charts.size()-1){
        this->currentPage++;
    }
    if (this->currentPage > 0){
        ui->prevButton->setEnabled(true);
    }
    if (this->currentPage == this->charts.size()-1){
        ui->nextButton->setDisabled(true);
    }
    ui->chartBrowser->setText(this->charts[this->currentPage]);
}

void MainWindow::on_tuningEdit_textEdited(const QString &arg1)
{
    ui->comboBox->setCurrentText(QString("Custom"));
    validateTuning();
}

void MainWindow::on_comboBox_currentTextChanged(const QString &arg1)
{
    if (arg1 == "Guitar"){
        ui->tuningEdit->setText(QString("EADGBE"));
        ui->spinBox->setValue(6);
    }
    else if (arg1 == "Bass"){
        ui->tuningEdit->setText((QString("EADG")));
        ui->spinBox->setValue(4);
    }
    else if (arg1 == "Ukulele"){
        ui->tuningEdit->setText((QString("GCEA")));
        ui->spinBox->setValue(4);
    }
    validateTuning();
}

