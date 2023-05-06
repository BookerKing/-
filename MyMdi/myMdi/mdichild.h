#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QWidget>

class MdiChild : public QTextEdit
{
    Q_OBJECT
public:
    explicit MdiChild(QWidget *parent = 0);
    void newFile();
    bool loadFile(const QString &filename);
    bool save();
    bool saveAs();
    bool saveFile(const QString &filename);
    QString userFriendlyCurrentFile();
    QString currentFile(){return curFile;}

protected:
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *e);

private slots:
    void documentWasModifiled();

private:
    bool maybeSave();
    void setCurrentFile(const QString &filename);
    QString curFile;
    bool isUntitled;
};

#endif // MDICHILD_H
