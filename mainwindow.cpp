#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QString>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SchedulerSim");
    this->quantum = 1;
    this->idCount = 0;
    this->i=1;
    this->pre = false;
    this->nonPre = false;
    ui->typeGb->setEnabled(false);
    ui->idEdit->setEnabled(false);
    ui->burstEdit->setEnabled(false);
    ui->arrivalEdit->setEnabled(false);
    ui->priorityEdit->setEnabled(false);
    ui->prevButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->ganttLayout->setSpacing(0);
    ui->timeLayout->setSpacing(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::checkValidity(bool *isPriorityOk){
    bool isIdInt = false, isBurstDouble = false, isArrivalDouble = false, isPriorityInt = false;
    for (int j = 1 ; j<(int)history.size();++j){
        history[j][0].toInt(&isIdInt);
        history[j][3].toInt(&isPriorityInt);
        history[j][2].toDouble(&isArrivalDouble);
        history[j][1].toDouble(&isBurstDouble);
        if (!isPriorityInt){
            *isPriorityOk = false;
        }
        if (!isIdInt|| !isBurstDouble || !isArrivalDouble){
            QMessageBox::critical(this, "Error", "Invaid Processes Information");
            return false;
        }
    }
    return true;
}


void MainWindow::schedule(){
    targetProcesses = {};
    timeSlots = {};
    for (auto pointer : labels){
        delete pointer;
    }
    labels= {};
    //------------------------------------------
    bool isPriortyOk = true;
    if (!checkValidity(&isPriortyOk)){
        return;
    }
    //------------------------------------------
    map<int, int> idMap;
    this->quantum = ui->quantumEdit->text().toDouble();
    for (int j=1; j<(int)history.size();++j){
        idMap[j-1] = history[j][0].toInt(); //zero-based indexing -> actual id
        targetProcesses.push_back(Process(j-1 ,history[j][3].toInt(),history[j][2].toDouble(), history[j][1].toDouble() ));
    }

    if (ui->fcfsRb->isChecked()){
        MainWindow::timeSlots = FCFS(targetProcesses);
    }
    else if (ui->rrRb->isChecked()){
         bool isQuantumOk = false;
         ui->quantumEdit->text().toInt(&isQuantumOk);
         if (!isQuantumOk){
              QMessageBox::critical(this, "Error", "Quantum must be an integer value.");
              return;
         }
         MainWindow::timeSlots = RoundRobin(targetProcesses, quantum);
    }
    else if (ui->sjfRb->isChecked() && ui->nonPreRb->isChecked()){
         MainWindow::timeSlots = SJF(targetProcesses);
    }
    else if (ui->sjfRb->isChecked() && ui->preRB->isChecked()){
         MainWindow::timeSlots = SJFPrem(targetProcesses);
    }
    else if (ui->priorityRb->isChecked() && ui->nonPreRb->isChecked() ){
         if (!isPriortyOk){
             QMessageBox::critical(this, "Error", "Priorities must be integer values.");
             return;
         }
         MainWindow::timeSlots = Priority(targetProcesses);
    }
    else if (ui->priorityRb->isChecked() && ui->preRB->isChecked()){
         if (!isPriortyOk){
             QMessageBox::critical(this, "Error", "Priorities must be integer values.");
             return;
         }
         MainWindow::timeSlots= PriorityPrem(targetProcesses);
    }
    else{
        QMessageBox::warning(this, "WARNING", "Missing scheduling information.");
    }

    //drawing gantt chart
    double init = 0;
    for (int i =0 ; i<(int)timeSlots.size();++i){
        if (timeSlots[i].start!=init){
            //creating the idle slots
              QLabel* slot = new QLabel();
              QLabel* time = new QLabel();
              slot->setStyleSheet("QLabel{border: 1px solid black; background: black;}");
              time->setText(QString::number(init));
              labels.push_back(slot);
              labels.push_back(time);
              int stretch = round(timeSlots[i].start - init);
              ui->ganttLayout->addWidget(slot, stretch);
              ui->timeLayout->addWidget(time, stretch);
        }
        QLabel* slot = new QLabel();
        QLabel* time = new QLabel();
        slot->setStyleSheet("QLabel{border: 1px solid black; background: white;}");
        slot->setText("P" + QString::number(idMap[timeSlots[i].id]));
        slot->setAlignment(Qt::AlignCenter);
        time->setText(QString::number(timeSlots[i].start));
        labels.push_back(slot);
        labels.push_back(time);
        int stretch = round(timeSlots[i].ending - timeSlots[i].start);
        ui->ganttLayout->addWidget(slot, stretch);
        ui->timeLayout->addWidget(time, stretch);
        init = timeSlots[i].ending;
    }
    //showing the last time
    QLabel* time = new QLabel();
    time->setText(QString::number(timeSlots[timeSlots.size()-1].ending));
    labels.push_back(time);
    ui->timeLayout->addWidget(time, 0);
    //calculating and showing the average
    ui->waitingLabel->setText("Average waiting time = " + QString::number(TimeSlot::avg));
}



//handling radio button options----
void MainWindow::on_fcfsRb_clicked()
{
    //disable type box
    if (ui->preRB->isChecked()){
        pre = true;
        nonPre = false;
    }
    if (ui->nonPreRb->isChecked()){
        pre = false;
        nonPre = true;
    }
    ui->nonPreRb->setAutoExclusive(false);
    ui->nonPreRb->setChecked(false);
    ui->nonPreRb->setAutoExclusive(true);
    ui->preRB->setAutoExclusive(false);
    ui->preRB->setChecked(false);
    ui->preRB->setAutoExclusive(true);
    ui->typeGb->setEnabled(false);
}


void MainWindow::on_sjfRb_clicked()
{
    ui->typeGb->setEnabled(true);
    if (pre){
        ui->preRB->setAutoExclusive(false);
        ui->preRB->setChecked(true);
        ui->preRB->setAutoExclusive(true);
    }
    if (nonPre){
        ui->nonPreRb->setAutoExclusive(false);
        ui->nonPreRb->setChecked(true);
        ui->nonPreRb->setAutoExclusive(true);
    }
}


void MainWindow::on_rrRb_clicked()
{
    //disable type box
    if (ui->preRB->isChecked()){
        pre = true;
        nonPre = false;
    }
    if (ui->nonPreRb->isChecked()){
        pre = false;
        nonPre = true;
    }
    ui->nonPreRb->setAutoExclusive(false);
    ui->nonPreRb->setChecked(false);
    ui->nonPreRb->setAutoExclusive(true);
    ui->preRB->setAutoExclusive(false);
    ui->preRB->setChecked(false);
    ui->preRB->setAutoExclusive(true);
    ui->typeGb->setEnabled(false);
}


void MainWindow::on_priorityRb_clicked()
{
    ui->typeGb->setEnabled(true);
    if (pre){
        ui->preRB->setAutoExclusive(false);
        ui->preRB->setChecked(true);
        ui->preRB->setAutoExclusive(true);
    }
    if (nonPre){
        ui->nonPreRb->setAutoExclusive(false);
        ui->nonPreRb->setChecked(true);
        ui->nonPreRb->setAutoExclusive(true);
    }
}
//--------------------------------------------


void MainWindow::on_nextButton_clicked()
{
    if (i > idCount){
        i = idCount;
    }
    //-------------------------------------------
    history[i] = {ui->idEdit->text(), ui->burstEdit->text(), ui->arrivalEdit->text(), ui->priorityEdit->text()};
    ++i;
    //clearing edit texts / retrieving
    if (i<=idCount){
        if (history[i].size() == 4){
            ui->idEdit->setText(history[i][0]);
            ui->burstEdit->setText(history[i][1]);
            ui->arrivalEdit->setText(history[i][2]);
            ui->priorityEdit->setText(history[i][3]);
        }
        else{
            ui->idEdit->setText("");
            ui->burstEdit->setText("");
            ui->arrivalEdit->setText("");
            ui->priorityEdit->setText("");
        }
    }
    if (i == idCount)
          ui->nextButton->setText("Shedule");
    if (i > idCount){
        MainWindow::schedule();
    }
}


void MainWindow::on_confirmButton_clicked()
{
    bool isInt = false;
    this->idCount = ui->countEdit->text().toInt(&isInt);
    if (!isInt){
        QMessageBox:: critical(this, "Error","Number of processes must be an integer value.");
        return;
    }
    if (idCount < 1){
        QMessageBox:: information(this, "info", "Number of proccesses must be greater than or equal one.");
        return;
    }
    if (idCount == 1){
        ui->nextButton->setText("Schedule");
    }
    ui->confirmButton->setEnabled(false);
    ui->countEdit->setEnabled(false);
    ui->idEdit->setEnabled(true);
    ui->burstEdit->setEnabled(true);
    ui->arrivalEdit->setEnabled(true);
    ui->priorityEdit->setEnabled(true);
    ui->prevButton->setEnabled(true);
    ui->nextButton->setEnabled(true);
    history.resize(idCount + 1);
}


void MainWindow::on_prevButton_clicked()
{
    if (i == 1)
        return;
    if (i > idCount){
        i = idCount;
    }
    history[i] = {ui->idEdit->text(), ui->burstEdit->text(), ui->arrivalEdit->text(), ui->priorityEdit->text()};
    --i;
    ui->idEdit->setText(history[i][0]);
    ui->arrivalEdit->setText(history[i][2]);
    ui->burstEdit->setText(history[i][1]);
    ui->priorityEdit->setText(history[i][3]);
    ui->nextButton->setText("Next");
}




void MainWindow::on_resetButton_clicked()
{

    this->quantum = 1;
    this->idCount = 0;
    this->i=1;
    this->pre = false;
    this->nonPre = false;
    ui->idEdit->setEnabled(false);
    ui->burstEdit->setEnabled(false);
    ui->arrivalEdit->setEnabled(false);
    ui->priorityEdit->setEnabled(false);
    ui->prevButton->setEnabled(false);
    ui->nextButton->setEnabled(false);
    ui->countEdit->setEnabled(true);
    ui->confirmButton->setEnabled(true);
    ui->idEdit->setText("");
    ui->burstEdit->setText("");
    ui->arrivalEdit->setText("");
    ui->priorityEdit->setText("");
    ui->countEdit->setText("");
    ui->waitingLabel->setText("");
    ui->nextButton->setText("Next");
    history.clear();
    history.resize(0);
    targetProcesses.clear();
    targetProcesses = {};
    timeSlots.clear();
    timeSlots = {};
    for (auto pointer : labels){
        delete pointer;
    }
    labels.clear();
    labels= {};
}

