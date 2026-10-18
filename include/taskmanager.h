#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QWidget>
#include <QProcess>
#include <QStandardItemModel>
#include <QDragEnterEvent>
#include <QDropEvent>

namespace Ui {
class TaskManager;
}

class TaskManager : public QWidget
{
    Q_OBJECT

public:
    explicit TaskManager(QWidget *parent = nullptr);
    ~TaskManager();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void loadProcesses();
    void updateProcessList();
    void endProcess();

private:
    Ui::TaskManager *ui;
    QProcess *processLister;
    QStandardItemModel *model;

    void setupModel();
    void setupConnections();
    bool confirmProcessKill(const QString &pid, const QString &name);
    void terminateProcessById(const QString &pid);
};

#endif // TASKMANAGER_H
