#include "taskmanager.h"
#include "ui_taskmanager.h"

#include <QMessageBox>
#include <QTimer>
#include <QMimeData>

TaskManager::TaskManager(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TaskManager),
      processLister(new QProcess(this)),
      model(new QStandardItemModel(this))
{
    ui->setupUi(this);
    setAcceptDrops(true);

    setupModel();
    setupConnections();

    loadProcesses();

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &TaskManager::loadProcesses);
    timer->start(5000);
}

TaskManager::~TaskManager()
{
    if (processLister->state() == QProcess::Running)
        processLister->kill();

    delete ui;
}

void TaskManager::setupModel()
{
#ifdef Q_OS_WIN
    model->setHorizontalHeaderLabels({"PID", "Process Name"});
#else
    model->setHorizontalHeaderLabels({"PID", "CPU %", "MEM %", "Command"});
#endif

    ui->processTable->setModel(model);
    ui->processTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->processTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->processTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->processTable->setDragEnabled(true);
}

void TaskManager::setupConnections()
{
    connect(ui->endProcessButton, &QPushButton::clicked,
            this, &TaskManager::endProcess);

    connect(processLister, &QProcess::readyReadStandardOutput,
            this, &TaskManager::updateProcessList);
}

void TaskManager::loadProcesses()
{
    if (processLister->state() == QProcess::Running)
        return;

    model->removeRows(0, model->rowCount());

#ifdef Q_OS_WIN
    processLister->start("tasklist", {"/FO", "CSV", "/NH"});
#else
    processLister->start("ps", {"aux"});
#endif
}

void TaskManager::updateProcessList()
{
    QByteArray output = processLister->readAllStandardOutput();
    QList<QByteArray> lines = output.split('\n');

    for (const QByteArray &line : lines)
    {
        if (line.trimmed().isEmpty())
            continue;

#ifdef Q_OS_WIN
        QList<QByteArray> parts = line.split(',');
        if (parts.size() < 2)
            continue;

        QString pid = parts[1].replace("\"", "");
        QString name = parts[0].replace("\"", "");

        QList<QStandardItem*> row;
        QStandardItem *pidItem = new QStandardItem(pid);
        pidItem->setData(pid, Qt::UserRole);

        row << pidItem << new QStandardItem(name);
        model->appendRow(row);

#else
        QList<QByteArray> parts = line.split(' ');
        parts.removeAll("");

        if (parts.size() < 11)
            continue;

        QString pid = parts[1];
        QString cpu = parts[2];
        QString mem = parts[3];
        QString cmd = parts[10];

        QList<QStandardItem*> row;
        QStandardItem *pidItem = new QStandardItem(pid);
        pidItem->setData(pid, Qt::UserRole);

        row << pidItem
            << new QStandardItem(cpu)
            << new QStandardItem(mem)
            << new QStandardItem(cmd);

        model->appendRow(row);
#endif
    }
}

void TaskManager::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasText() &&
        event->source() == ui->processTable)
    {
        event->acceptProposedAction();
    }
}

void TaskManager::dropEvent(QDropEvent *event)
{
    if (ui->endProcessButton->geometry().contains(event->pos()))
    {
        QString pid = event->mimeData()->text().trimmed();
        QString name = "Process";

        if (confirmProcessKill(pid, name))
            terminateProcessById(pid);

        event->acceptProposedAction();
    }
}

void TaskManager::endProcess()
{
    QModelIndex index = ui->processTable->currentIndex();
    if (!index.isValid())
    {
        QMessageBox::warning(this, "No Selection",
                             "Please select a process first.");
        return;
    }

    QString pid = model->item(index.row(), 0)->data(Qt::UserRole).toString();
    QString name = model->item(index.row(), 1)->text();

    if (confirmProcessKill(pid, name))
        terminateProcessById(pid);
}

bool TaskManager::confirmProcessKill(const QString &pid, const QString &name)
{
    QMessageBox::StandardButton reply =
        QMessageBox::question(this,
                              "Confirm Termination",
                              "Terminate " + name + " (PID " + pid + ")?",
                              QMessageBox::Yes | QMessageBox::No);

    return reply == QMessageBox::Yes;
}

void TaskManager::terminateProcessById(const QString &pid)
{
    QProcess killer;

#ifdef Q_OS_WIN
    killer.start("taskkill", {"/PID", pid, "/F"});
#else
    killer.start("kill", {pid});
#endif

    killer.waitForFinished(2000);

    loadProcesses();
}
