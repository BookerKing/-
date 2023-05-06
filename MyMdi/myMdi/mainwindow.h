#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MdiChild;
class QMdiSubWindow;
class QSignalMapper;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateMenus();

    MdiChild *createMdiChild();

    void on_actionNew_triggered();

    void setActiveSubWindow(QWidget *window);

    void updateWindowMenu();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionUndo_triggered();

    void on_actionClose_triggered();

    void on_actionExit_triggered();

    void showTextRowAndCol();

private:
    Ui::MainWindow *ui;
    QAction *actionSeparator;
    MdiChild *activeMdiChild();
    QMdiSubWindow *findMdiChild(const QString &filename);
    QSignalMapper *windowMapper;
    void readSettings();
    void writeSettings();
    void initWindow();

protected:
    void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
