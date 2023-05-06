#include "mdichild.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QCloseEvent>
#include <QPushButton>
#include <QMenu>

MdiChild::MdiChild(QWidget *parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

void MdiChild::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    curFile = tr("Unnamed Document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]" + tr("-Multiple Document Editor"));
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModifiled()));
}

void MdiChild::documentWasModifiled()
{
    setWindowModified(document()->isModified());
}

bool MdiChild::loadFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Multiple Document Editor"), tr("Unable to read file%1: \n%2.").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    setCurrentFile(filename);
    connect(document(), SIGNAL(contentsChange()), this, SLOT(documentWasModifiled()));
    return true;
}

void MdiChild::setCurrentFile(const QString &filename)
{
    curFile = QFileInfo(filename).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString MdiChild::userFriendlyCurrentFile()
{
    return QFileInfo(curFile).fileName();
}

bool MdiChild::save()
{
    if(isUntitled)
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool MdiChild::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("save as"), curFile);
    if(filename.isEmpty())
        return false;
    return saveFile(filename);
}

bool MdiChild::saveFile(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, tr("Multiple Document Editor"), tr("Unable to write to file %1:\n%2.").arg(filename).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();
    setCurrentFile(filename);
    return true;
}

void MdiChild::closeEvent(QCloseEvent *event)
{
    if(maybeSave()){
        event->accept();
    }else{
        event->ignore();
    }
}

bool MdiChild::maybeSave()
{
    if(document()->isModified())
    {
        QMessageBox box;
        box.setWindowTitle(QString::fromLocal8Bit("Multiple Document Editor"));
        box.setText(tr("Do you want to save changes to '%1'?").arg(userFriendlyCurrentFile()));
        box.setIcon(QMessageBox::Warning);
        QPushButton *yesBtn = box.addButton(tr("YES(&Y)"), QMessageBox::YesRole);
        box.addButton(tr("NO(&N)"), QMessageBox::NoRole);
        QPushButton *cancelBtn = box.addButton(tr("CANCLE"), QMessageBox::RejectRole);
        box.exec();                                     //Ä£Ì¬´°¿Ú
        if(box.clickedButton() == yesBtn)
            return save();
        else if(box.clickedButton() == cancelBtn)
            return false;
    }
    return true;
}

void MdiChild::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *menu = new QMenu;
    QAction *undo = menu->addAction(tr("undo(&U)"), this, SLOT(undo()), QKeySequence::Undo);
    undo->setEnabled(document()->isUndoAvailable());
    QAction *redo = menu->addAction(tr("redo(&R)"), this, SLOT(redo()), QKeySequence::Redo);
    redo->setEnabled(document()->isRedoAvailable());
    menu->addSeparator();
    QAction *cut = menu->addAction(tr("cut(&T)"), this, SLOT(cut()), QKeySequence::Cut);
    cut->setEnabled(QTextCursor().hasSelection());
    QAction *copy = menu->addAction(tr("copy(&C)"), this, SLOT(copy()), QKeySequence::Copy);
    copy->setEnabled(QTextCursor().hasSelection());
    menu->addAction(tr("paste(&P)"), this, SLOT(paste()), QKeySequence::Paste);
    QAction *clear = menu->addAction(tr("clear"), this, SLOT(clear()));
    clear->setEnabled(!document()->isEmpty());
    menu->addSeparator();
    QAction *select = menu->addAction(tr("selectAll"), this, SLOT(selectAll()), QKeySequence::SelectAll);
    select->setEnabled(!document()->isEmpty());
    menu->exec(e->globalPos());
    delete menu;
}
