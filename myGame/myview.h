#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include "mybox.h"
#include <QMediaPlayer>

class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = 0);
public slots:
    void startGame();
    void clearFullRows();
    void moveBox();
    void gameOver();
    void restartGame();
    void finishGame();
    void pauseGame();
    void returnGame();
    void aboutToFinish();
    void controlVolume1(int);
    void controlVolume2(int);
private:
    BoxGroup *boxGroup;
    BoxGroup *nextBoxGroup;
    QGraphicsLineItem *topLine;
    QGraphicsLineItem *bottomLine;
    QGraphicsLineItem *leftLine;
    QGraphicsLineItem *rightLine;
    qreal gameSpeed;
    QList<int> rows;
    void initView();
    void initGame();
    void updateScore(const int fullRowNum = 0);
    QGraphicsTextItem *gameScoreText;
    QGraphicsTextItem *gameLevelText;
    QGraphicsWidget *maskWidget;
    QGraphicsWidget *startButton;
    QGraphicsWidget *finishButton;
    QGraphicsWidget *restartButton;
    QGraphicsWidget *pauseButton;
    QGraphicsWidget *optionButton;
    QGraphicsWidget *returnButton;
    QGraphicsWidget *helpButton;
    QGraphicsWidget *exitButton;
    QGraphicsWidget *showMenuButton;

    QGraphicsTextItem *gameWelcomeText;
    QGraphicsTextItem *gamePauseText;
    QGraphicsTextItem *gameOverText;
    QMediaPlayer *backgroundMusic;
    QMediaPlayer *clearRowSound;
    QAudioOutput *audio1;
    QAudioOutput *audio2;

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // MYVIEW_H
