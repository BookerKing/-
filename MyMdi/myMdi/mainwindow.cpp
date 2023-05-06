#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mdichild.h"
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QSignalMapper>
#include <QSettings>
#include <QCloseEvent>
#include <QMenu>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    actionSeparator = new QAction(this);
    actionSeparator->setSeparator(true);
    updateMenus();
    connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
    updateWindowMenu();
    connect(ui->menu_W, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    readSettings();
    initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    ui->actionSave->setEnabled(hasMdiChild);
    ui->actionSaveAs->setEnabled(hasMdiChild);
    ui->actionPaste->setEnabled(hasMdiChild);
    ui->actionClose->setEnabled(hasMdiChild);
    ui->actionCloseAll->setEnabled(hasMdiChild);
    ui->actionTitle->setEnabled(hasMdiChild);
    ui->actionCascade->setEnabled(hasMdiChild);
    ui->actionNext->setEnabled(hasMdiChild);
    ui->actionPrevious->setEnabled(hasMdiChild);
    actionSeparator->setVisible(hasMdiChild);
    bool hasSelection = (activeMdiChild() && activeMdiChild()->textCursor().hasSelection());
    ui->actionCut->setEnabled(hasSelection);
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionUndo->setEnabled(activeMdiChild() && activeMdiChild()->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(activeMdiChild() && activeMdiChild()->document()->isRedoAvailable());
}

MdiChild *MainWindow::activeMdiChild()
{
    if(QMdiSubWindow *activeSubWindow = ui->mdiArea->activeSubWindow())
        return qobject_cast<MdiChild *>(activeSubWindow->widget());
    return 0;
}

void MainWindow::on_actionNew_triggered()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();         //非模态窗口
}

MdiChild * MainWindow:: createMdiChild()
{
    MdiChild *child = new MdiChild;
    ui->mdiArea->addSubWindow(child);
    connect(child, SIGNAL(copyAvailable(bool)), ui->actionCut, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)), ui->actionCopy, SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    connect(child->document(), SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(cursorPositionChanged()), this, SLOT(showTextRowAndCol()));
    return child;
}


void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if(!filename.isEmpty())
    {
        QMdiSubWindow *existing = findMdiChild(filename);
        if(existing){
            ui->mdiArea->setActiveSubWindow(existing);
            return;
        }
        MdiChild *child = createMdiChild();
        if(child->loadFile(filename)){
            ui->statusbar->showMessage(tr("Successfully opened file"), 2000);
            child->show();
        }else{
            child->close();
        }
    }
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &filename)
{
    QString canonicalFilePath = QFileInfo(filename).canonicalFilePath();
    foreach (QMdiSubWindow *window, ui->mdiArea->subWindowList()) {
        MdiChild *mdiChild = qobject_cast<MdiChild *>(window->widget());
        if(mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::setActiveSubWindow(QWidget *window)
{
    if(!window)
        return;
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MainWindow::updateWindowMenu()
{
    ui->menu_W->clear();
    ui->menu_W->addAction(ui->actionClose);
    ui->menu_W->addAction(ui->actionCloseAll);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->actionTitle);
    ui->menu_W->addAction(ui->actionCascade);
    ui->menu_W->addAction(ui->actionNext);
    ui->menu_W->addAction(ui->actionPrevious);
    ui->menu_W->addAction(actionSeparator);
    QList<QMdiSubWindow *> windows = ui->mdiArea->subWindowList();
    actionSeparator->setVisible(!windows.isEmpty());

    for(int i = 0; i < windows.size(); i++)
    {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());
        QString text;
        if(i < 9)
        {
            text = tr("&%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());
        }else{
            text = tr("%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action = ui->menu_W->addAction(text);
        action->setCheckable(true);
        action->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(activeMdiChild() && activeMdiChild()->save())
        ui->statusbar->showMessage(tr("File saved successfully"), 2000);
}


void MainWindow::on_actionUndo_triggered()
{
    if(activeMdiChild())
        activeMdiChild()->undo();
}


void MainWindow::on_actionClose_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->currentSubWindow())
        event->ignore();
    else{
        writeSettings();
        event->accept();
    }
}

void MainWindow::writeSettings()
{
    QSettings settings("File_Edit", "myMdi");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::readSettings()
{
    QSettings settings("File_Edit", "myMdi");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void MainWindow::on_actionExit_triggered()
{
    qApp->closeAllWindows();
}

void MainWindow::showTextRowAndCol()
{
    if(activeMdiChild()){
        int rowNum = activeMdiChild()->textCursor().blockNumber()+1;
        int columNum = activeMdiChild()->textCursor().columnNumber()+1;
        ui->statusbar->showMessage(tr("%1row %2colum").arg(rowNum).arg(columNum), 2000);
    }
}

void MainWindow::initWindow()
{
    setWindowTitle(QString::fromLocal8Bit("多文档编辑器"));
    ui->mainToolBar->setWindowTitle(tr("toolbar"));
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->statusbar->showMessage(tr("Welcome to the Multiple Document Editor"));
    QLabel *label = new QLabel(this);
    label->setText(tr("personal Editor"));
    label->setTextFormat(Qt::RichText);
    ui->statusbar->addPermanentWidget(label);
    ui->actionNew->setStatusTip(tr("Create a file"));
}
