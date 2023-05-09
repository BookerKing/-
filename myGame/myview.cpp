#include "myview.h"
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QTimer>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QLabel>
#include <QFileInfo>
#include <QWidget>
#include <QAudioOutput>
#include <QSlider>


static const qreal INITSPEED = 500;
static const QString SOUNDPATH = "./sound/";

MyView::MyView::MyView(QWidget *parent): QGraphicsView(parent)
{
    initView();
}

void MyView::initView()
{
    setRenderHint(QPainter::Antialiasing);
    setCacheMode(CacheBackground);
    setWindowTitle(tr("MyBox Block Game"));
    setWindowIcon(QIcon(":/images/icon.png"));
    setMinimumSize(810, 510);
    setMaximumSize(810, 510);
    QGraphicsScene *scene = new QGraphicsScene;
    scene->setSceneRect(5, 5, 800, 500);
    scene->setBackgroundBrush(QPixmap(":/images/background.png"));
    setScene(scene);
    topLine = scene->addLine(197, 10, 403, 10);     //(x1, y1, x2, y2)
    bottomLine = scene->addLine(197, 453, 403, 453);
    leftLine = scene->addLine(197, 10, 197, 453);
    rightLine = scene->addLine(403, 10, 403, 453);
    boxGroup = new BoxGroup;
    connect(boxGroup, SIGNAL(needNewBox()), this, SLOT(clearFullRows()));
    connect(boxGroup, SIGNAL(gameFinished()), this, SLOT(gameOver()));
    scene->addItem(boxGroup);
    nextBoxGroup = new BoxGroup;
    scene->addItem(nextBoxGroup);
    gameScoreText = new QGraphicsTextItem();
    gameScoreText->setFont(QFont("Times", 20, QFont::Bold));
    gameScoreText->setPos(650, 350);
    gameLevelText = new QGraphicsTextItem();
    gameLevelText->setFont(QFont("Times", 30, QFont::Bold));
    gameLevelText->setPos(20, 150);
    scene->addItem(gameLevelText);
    scene->addItem(gameScoreText);
//    startGame();

    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 80)));
    mask->resize(900, 600);

    maskWidget = scene->addWidget(mask);
    maskWidget->setPos(-50, -50);
    maskWidget->setZValue(1);

    QWidget *option = new QWidget;

    QPushButton *option_close_button = new QPushButton(tr("close"), option);

    QPalette palette;
    palette.setColor(QPalette::ButtonText, Qt::black);
    option_close_button->setPalette(palette);

    option_close_button->move(120, 300);
    connect(option_close_button, SIGNAL(clicked()), option, SLOT(hide()));

    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0, 0, 0, 180)));
    option->resize(300, 400);
    QGraphicsWidget *option_widget = scene->addWidget(option);
    option_widget->setPos(250, 50);
    option_widget->setZValue(3);
    option_widget->hide();


    QWidget *help = new QWidget;
    QPushButton *help_close_button = new QPushButton(tr("close"), help);
    help_close_button->setPalette(palette);
    help_close_button->move(120, 300);
    connect(help_close_button, SIGNAL(clicked()), help, SLOT(hide()));

    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(300, 400);
    QGraphicsWidget *help_widget = scene->addWidget(help);
    help_widget->setPos(250, 50);
    help_widget->setZValue(3);
    help_widget->hide();

    gameWelcomeText = new QGraphicsTextItem(0);
    gameWelcomeText->setHtml(tr("<font color=blue>Tetris Game</font>"));
    gameWelcomeText->setFont(QFont("Times", 30, QFont::Bold));
    gameWelcomeText->setPos(250, 100);
    gameWelcomeText->setZValue(2);

    gamePauseText = new QGraphicsTextItem(0);
    gamePauseText->setHtml(tr("<font color=green>Game paused!</font>"));
    gamePauseText->setFont(QFont("Times", 30, QFont::Bold));
    gamePauseText->setPos(300, 100);
    gamePauseText->setZValue(2);
    gamePauseText->hide();

    gameOverText = new QGraphicsTextItem();
    gameOverText->setHtml(tr("<font color=green>GAME¡¡OVER£¡</font>"));
    gameOverText->setFont(QFont("Times", 30, QFont::Bold));
    gameOverText->setPos(320, 100);
    gameOverText->setZValue(2);
    gameOverText->hide();

    scene->addItem(gameWelcomeText);
    scene->addItem(gamePauseText);
    scene->addItem(gameOverText);


    QPushButton *button1 = new QPushButton(tr("Start"));
    QPushButton *button2 = new QPushButton(tr("Option"));
    QPushButton *button3 = new QPushButton(tr("Help"));
    QPushButton *button4 = new QPushButton(tr("Exit"));
    QPushButton *button5 = new QPushButton(tr("Restart"));
    QPushButton *button6 = new QPushButton(tr("Pause"));
    QPushButton *button7 = new QPushButton(tr("Main Menu"));
    QPushButton *button8 = new QPushButton(tr("Return To The Game"));
    QPushButton *button9 = new QPushButton(tr("Game Over"));

    connect(button1, SIGNAL(clicked()), this, SLOT(startGame()));
    connect(button2, SIGNAL(clicked()), option, SLOT(show()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(button5, SIGNAL(clicked()), this, SLOT(restartGame()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pauseGame()));
    connect(button7, SIGNAL(clicked()), this, SLOT(showMenuButton()));
    connect(button8, SIGNAL(clicked()), this, SLOT(returnGame()));
    connect(button9, SIGNAL(clicked()), this, SLOT(finishGame()));

    startButton = scene->addWidget(button1);
    optionButton = scene->addWidget(button2);
    helpButton = scene->addWidget(button3);
    exitButton = scene->addWidget(button4);
    restartButton = scene->addWidget(button5);
    pauseButton = scene->addWidget(button6);
    showMenuButton = scene->addWidget(button7);
    returnButton = scene->addWidget(button8);
    finishButton = scene->addWidget(button9);


    startButton->setPos(370, 200);
    optionButton->setPos(370, 250);
    helpButton->setPos(370, 300);
    exitButton->setPos(370, 350);
    restartButton->setPos(600, 150);
    pauseButton->setPos(600, 200);
    showMenuButton->setPos(600, 250);
    returnButton->setPos(370, 200);
    finishButton->setPos(370, 250);


    startButton->setZValue(2);
    optionButton->setZValue(2);
    helpButton->setZValue(2);
    exitButton->setZValue(2);
    restartButton->setZValue(2);
    returnButton->setZValue(2);
    finishButton->setZValue(2);


    restartButton->hide();
    finishButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    returnButton->hide();

    backgroundMusic = new QMediaPlayer(this);
    clearRowSound = new QMediaPlayer(this);
    audio1 = new QAudioOutput(this);
    audio2 = new QAudioOutput(this);
    backgroundMusic->setAudioOutput(audio1);
    clearRowSound->setAudioOutput(audio2);
    QSlider *volume1 = new QSlider(Qt::Horizontal, option);
    QSlider *volume2 = new QSlider(Qt::Horizontal, option);
    volume1->setTickPosition(QSlider::TicksRight);
    volume2->setTickPosition(QSlider::TicksRight);
    volume1->setValue(50);
    volume2->setValue(50);
    connect(volume1, SIGNAL(sliderMoved(int)), this, SLOT(controlVolume1(int)));
    connect(volume2, SIGNAL(sliderMoved(int)), this, SLOT(controlVolume2(int)));
    QLabel *volumeLabel1 = new QLabel(tr("music: "), option);
    QLabel *volumeLabel2 = new QLabel(tr("sound effects: "), option);
    volume1->move(140, 100);
    volume2->move(140, 200);
    volumeLabel1->move(60, 105);
    volumeLabel2->move(60, 205);
    connect(backgroundMusic, SIGNAL(aboutToFinish()), this, SLOT(aboutToFinish()));
    connect(clearRowSound, SIGNAL(finished()), clearRowSound, SLOT(stop()));
    backgroundMusic->setSource(QUrl::fromLocalFile(SOUNDPATH+"background.mp3"));
    clearRowSound->setSource(QUrl::fromLocalFile(SOUNDPATH+"clearRow.mp3"));
    backgroundMusic->play();
}

void MyView::startGame()
{
    gameWelcomeText->hide();
    startButton->hide();
    optionButton->hide();
    helpButton->hide();
    exitButton->hide();
    maskWidget->hide();
    initGame();
}

void MyView::initGame()
{
    scene()->setBackgroundBrush(QPixmap(":/images/background01.png"));
    gameScoreText->setHtml(tr("<font color = red>0</font>"));
    gameLevelText->setHtml(tr("<font color = white>NO.<br>1</font>"));
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    gameScoreText->show();
    gameLevelText->show();
    topLine->show();
    bottomLine->show();
    leftLine->show();
    rightLine->show();
    boxGroup->show();
    boxGroup->createBox(QPointF(300, 70));
    boxGroup->setFocus();
    boxGroup->startTimer(INITSPEED);
    gameSpeed = INITSPEED;
    nextBoxGroup->createBox(QPointF(500, 70));
    backgroundMusic->setSource(QUrl::fromLocalFile(SOUNDPATH+"background01.mp3"));
    backgroundMusic->play();
}

void MyView::clearFullRows()
{
    for(int y = 429; y > 50; y -= 20)
    {
        QList<QGraphicsItem *> list = scene()->items(QRectF(199, y, 202, 22), Qt::ContainsItemShape);
        if(list.count() == 10)
        {
            foreach (QGraphicsItem *item, list)
            {
                OneBox *box = (OneBox*)item;
            //                box->deleteLater();
            QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
            box->setGraphicsEffect(blurEffect);
            QPropertyAnimation *animation = new QPropertyAnimation(box, "scale");
            animation->setEasingCurve(QEasingCurve::OutBounce);
            animation->setDuration(250);
            animation->setStartValue(4);
            animation->setEndValue(0.25);
            animation->start(QAbstractAnimation::DeleteWhenStopped);
            connect(animation, SIGNAL(finished()), box, SLOT(deleteLater()));
        }
        rows << y;
    }
}
    if(rows.count() > 0)
    {
        clearRowSound->play();
        QTimer::singleShot(400, this, SLOT(moveBox()));
    }
    else
    {
        boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
        nextBoxGroup->clearBoxGroup(true);
        nextBoxGroup->createBox(QPointF(500, 70));
    }
}


void MyView::moveBox()
{
    for(int i = rows.count(); i > 0; i--)
    {
        int row = rows.at(i - 1);
        foreach (QGraphicsItem *item, scene()->items(QRectF(199, 49, 202, row - 47), Qt::ContainsItemShape))
        {
             item->moveBy(0, 20);
        }
    }
    updateScore(rows.count());
    rows.clear();
    boxGroup->createBox(QPointF(300, 70), nextBoxGroup->getCurrentShape());
    nextBoxGroup->clearBoxGroup(true);
    nextBoxGroup->createBox(QPointF(500, 70));
}

void MyView::updateScore(const int fullRowNUm)
{
    int score = fullRowNUm * 100;
    int currentScore = gameScoreText->toPlainText().toInt();
    currentScore += score;
    gameScoreText->setHtml(tr("<font color = red>%1</font>").arg(currentScore));
    if(currentScore < 500){

    }else if(currentScore < 1000){
        gameLevelText->setHtml(tr("<font color = white>NO.<br>2</font>"));
        scene()->setBackgroundBrush(QPixmap(":/images/background02.png"));
        gameSpeed = 300;
        boxGroup->stopTimer();
        boxGroup->startTimer(gameSpeed);
        if(QFileInfo(backgroundMusic->source().fileName()).baseName() != "background02"){
            backgroundMusic->setSource(QUrl::fromLocalFile(SOUNDPATH+"background02.mp3"));
            backgroundMusic->play();
        }
    }else{

    }
}

void MyView::gameOver()
{
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gameOverText->show();
    restartButton->setPos(370, 200);
    finishButton->show();
}

void MyView::restartGame()
{
    maskWidget->hide();
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);

    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect,Qt::DescendingOrder)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }
    initGame();
}

void MyView::finishGame()
{
    gameOverText->hide();
    finishButton->hide();
    restartButton->setPos(600, 150);
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    gameScoreText->hide();
    gameLevelText->hide();
    topLine->hide();
    bottomLine->hide();
    leftLine->hide();
    rightLine->hide();

    nextBoxGroup->clearBoxGroup(true);
    boxGroup->clearBoxGroup();
    boxGroup->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect,Qt::DescendingOrder)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }

    maskWidget->show();
    gameWelcomeText->show();
    startButton->show();
    optionButton->show();
    helpButton->show();
    exitButton->show();
    scene()->setBackgroundBrush(QPixmap(":/images/background.png"));
    backgroundMusic->setSource(QUrl::fromLocalFile(SOUNDPATH+"background.mp3"));
    backgroundMusic->play();
}

void MyView::pauseGame()
{
    boxGroup->stopTimer();
    restartButton->hide();
    pauseButton->hide();
    showMenuButton->hide();
    maskWidget->show();
    gamePauseText->show();
    returnButton->show();
}

void MyView::returnGame()
{
    returnButton->hide();
    gamePauseText->hide();
    maskWidget->hide();
    restartButton->show();
    pauseButton->show();
    showMenuButton->show();
    boxGroup->startTimer(gameSpeed);
}

void MyView::keyPressEvent(QKeyEvent *event)
{
    if(pauseButton->isVisible())
        boxGroup->setFocus();
    else
        boxGroup->clearFocus();
    QGraphicsView::keyPressEvent(event);
}

void MyView::aboutToFinish()
{
    backgroundMusic->loops();
}

void MyView::controlVolume1(int position)
{
    audio1->setVolume(position/100.0);
}

void MyView::controlVolume2(int position)
{
    audio2->setVolume(position/100.0);
}
