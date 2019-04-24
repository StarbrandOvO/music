#include "widget.h"
#include "ui_widget.h"
#include<QFile>
#include<QDebug>
#include<QPalette>
#include <QPixmap>
#include <QFile>
#include <QPainter>
#include <QFileDialog>
#include <QUrl>
#include <QMessageBox>
#include <QFileInfo>
#include<QMediaMetaData>
#include<QTime>
#include<QDesktopWidget>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

//    QAction* CollectionAction = new QAction(QIcon(":/images/heart01.png"),"收藏歌曲",this);
//    QAction* DeleteAction = new QAction(QIcon(":/images/heart01.png"),"删除",this);
//    addAction(CollectionAction);
//    addAction(DeleteAction);
//    setContextMenuPolicy(Qt::ActionsContextMenu);

    setMinimumSize(this->width(),this->height());
    setMaximumSize(this->width(),this->height());
    setWindowFlags(Qt::FramelessWindowHint);//无边框

    lpressed = false;
    ui->logo_label->setScaledContents(true);
    ui->close_Btn->setGeometry(this->width()-30,0,30,30);
    ui->min_Btn->setGeometry(this->width()-60,0,30,30);
    ui->logo_label->setGeometry(8,5,25,25);
    ui->logo_label->setPixmap(QPixmap(":/images/logo.png"));
    ui->play_Btn->setObjectName("play_Btn");
    ui->last_Btn->setObjectName("last_Btn");
    ui->next_Btn->setObjectName("next_Btn");
    ui->seekSlider->setObjectName("seekSlider");
    ui->volumeControl->setObjectName("volumeControl");
    ui->muteButton->setObjectName("muteButton");
    ui->playPattern_Btn->setObjectName("playPattern_Btn");
    ui->add_Btn->setObjectName("add_Btn");
    ui->list->setObjectName("list");
    ui->search_LineEdit->setObjectName("search_LineEdit");
    ui->search_Btn->setObjectName("search_Btn");
    ui->close_Btn->setObjectName("close_Btn");
    ui->min_Btn->setObjectName("min_Btn");
//    ui->Collection_Btn->setObjectName("Collection_Btn");
    QPalette pac;//创建调色板
    pac.setColor(QPalette::WindowText,QColor(0,0,0,0));
    ui->list -> setFont(QFont("Times",10,QFont::Bold));
    ui->list -> setPalette(pac);
    ui->list ->setStyleSheet("background: rgba(0,0,0,0);"
                             "background-color: rgb(255, 0, 0, 0);"
                             "border: 1px solid black;"
                             "margin:10px 10px 10px 10px;");
    ui->Lrc_list->setFont(QFont("Times",15,QFont::Bold));
    ui->Lrc_list->setPalette(pac);
    ui->Lrc_list ->setStyleSheet("background: rgba(0,0,0,0);"
                             "background-color: rgb(255, 0, 0, 0);"
                             "border: 1px solid black;"
                             "margin:10px 10px 10px 10px;");
    isPlay = false;                                             //正在播放为false
    isMute = false;                                             //是否静音为false
    music_position = 0;                                         //歌曲播放进度为0
    music_volume = 50;                                          //初始化音量为50
    playPattern = 0;                                            //播放顺序
    playList = new QMediaPlaylist;                              //播放列表
    playList->setPlaybackMode(QMediaPlaylist::Sequential);      //初始化播放顺序为顺序播放

    player = new QMediaPlayer;

    player->setVolume(music_volume);
    setBtnStyle();                                              //初始化控件qss

    ui->volumeControl->setMinimum(0);                           //音量的进度条
    ui->volumeControl->setMaximum(100);

    ui->volumeControl->setValue(music_volume);

    ui->time_label->setText("00:00:00");
    ui->now_time_label->setText("00:00:00");

    ui->play_Btn->setToolTip(QString("播放"));
    ui->playPattern_Btn->setToolTip(QString("顺序播放"));
    ui->add_Btn->setToolTip(QString("添加"));
    ui->playList_label->setFont(QFont("宋体",18,QFont::Bold));
    ui->lrcList_label->setFont(QFont("宋体",18,QFont::Bold));
    ui->nowplay_label->setFont(QFont("宋体",10,QFont::Normal));
    ui->tital_label->setFont(QFont("宋体",20,QFont::Normal));
    titalStr = ui->tital_label->text();

    QTimer* moveTital = new QTimer(this);           //计时器用来移动标题
    titalweizhi = 0;

    connect(moveTital,SIGNAL(timeout()),this,SLOT(doMoveTital()));
    moveTital->start(1000);

    KuGouSearch = new KuGou(this);

    //新建QSystemTrayIcon对象
    mSysTrayIcon = new QSystemTrayIcon(this);
    //新建托盘要显示的icon
    QIcon ic = QIcon(":/images/logo.png");
    //将icon设到QSystemTrayIcon对象中
    mSysTrayIcon->setIcon(ic);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mSysTrayIcon->setToolTip(QObject::trUtf8("音乐"));

    //在系统托盘显示此对象
    mSysTrayIcon->show();

    connect(KuGouSearch,SIGNAL(nameAdd(QString)),this,SLOT(listAdd(QString)));
    connect(KuGouSearch,SIGNAL(lrcAdd(QString)),this,SLOT(lrcStrAdd(QString)));
    connect(KuGouSearch,SIGNAL(mediaAdd(QString)),this,SLOT(urlListAdd(QString)));

    connect(player,SIGNAL(metaDataAvailableChanged(bool)),this,SLOT(on_player_Data_change()));
    connect(player,SIGNAL(positionChanged(qint64)),this,SLOT(on_player_position_change(qint64)));
    connect(player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(on_mediaStatusChanged(QMediaPlayer::MediaStatus)));

    nowIsFirst = true;


    myLrc_widget = new lrc_widget();
    myLrc_widget->show();
    QDesktopWidget* pDesktopWidget = QApplication::desktop();
        //获取可用桌面大小
    QRect deskRect = QApplication::desktop()->availableGeometry();

    myLrc_widget->move(deskRect.width()/3,deskRect.height()-150);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter myPainter(this);
    myPainter.setOpacity(0.9);          //背景图片透明度
    myPainter.drawPixmap(0,0,this->width(),this->height(),QPixmap(":/images/BJ.jpg"));
}

void Widget::setBtnStyle()
{
    QFile file(":/My_MusicPlayer.qss");
    file.open(QFile::ReadOnly);
    this -> setStyleSheet(QObject::tr(file.readAll()));
    file.close();
}

void Widget::on_play_Btn_clicked()                              //播放按钮
{
    if(!playList->isEmpty())
    {
        isPlay = !isPlay;
        if(!isPlay)
        {
            ui->play_Btn->setToolTip(QString("播放"));
            ui->play_Btn->setStyleSheet("QPushButton#play_Btn:hover{border-image: url(:/images/play5.png);}"
                                        "QPushButton#play_Btn:!hover{border-image: url(:/images/play4.png);}"
                                        "QPushButton#play_Btn:pressed{border-image: url(:/images/play5.png);}");
            player->pause();
            myLrc_widget->stopLrc();
        }
        else
        {
            ui->play_Btn->setToolTip(QString("暂停"));
            ui->play_Btn->setStyleSheet("QPushButton#play_Btn:hover{border-image: url(:/images/play2.png);}"
                                        "QPushButton#play_Btn:!hover{border-image: url(:/images/play1.png);}"
                                        "QPushButton#play_Btn:pressed{border-image: url(:/images/play3.png);}");
            player->setPosition(music_position);
            player->play();
            myLrc_widget->startLrc();
        }
    }
    else
    {
        QMessageBox::warning(this,"提示: ","播放列表为空!\n请先添加歌曲!");
    }
}

void Widget::on_muteButton_clicked()                            //是否静音
{
    isMute = !isMute;
    if(!isMute)
    {
        ui->muteButton->setStyleSheet("QPushButton#muteButton:hover{border-image: url(:/images/sound.png);}"
                                    "QPushButton#muteButton:!hover{border-image: url(:/images/sound.png);}"
                                    "QPushButton#muteButton:pressed{border-image: url(:/images/nosound_dark.png);}");
        ui->volumeControl->setValue(100);
    }
    else
    {
        ui->muteButton->setStyleSheet("QPushButton#muteButton:hover{border-image: url(:/images/nosound_dark.png);}"
                                    "QPushButton#muteButton:!hover{border-image: url(:/images/nosound_light.png);}"
                                    "QPushButton#muteButton:pressed{border-image: url(:/images/sound.png);}");
        ui->volumeControl->setValue(0);
    }
}

void Widget::on_volumeControl_valueChanged(int value)           //调节音量
{
    if(value == 0)
    {
        ui->muteButton->setStyleSheet("QPushButton#muteButton:hover{border-image: url(:/images/nosound_dark.png);}"
                                    "QPushButton#muteButton:!hover{border-image: url(:/images/nosound_light.png);}"
                                    "QPushButton#muteButton:pressed{border-image: url(:/images/sound.png);}");
    }
    else
    {
        ui->muteButton->setStyleSheet("QPushButton#muteButton:hover{border-image: url(:/images/sound.png);}"
                                    "QPushButton#muteButton:!hover{border-image: url(:/images/sound.png);}"
                                    "QPushButton#muteButton:pressed{border-image: url(:/images/nosound_dark.png);}");
    }
    music_volume = value;
    player->setVolume(music_volume);
}

void Widget::on_add_Btn_clicked()       //添加歌曲
{
    QString songFileName,songName;
    QFileInfo info;
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("打开文件"),"",tr("music(*.mp3)"));
    if(!fileNames.isEmpty())
    {
        bool add = true;
    }
    for(int i = 0; i < fileNames.size(); i ++)
    {
        playList -> addMedia(QUrl::fromLocalFile(fileNames.at(i)));//将音乐文件逐一加入到播放列表

        songFileName = fileNames.at(i);
        info = QFileInfo(songFileName);
        songName = info.fileName();
        ui->list->addItem(songName);//添加歌曲名到界面的歌曲列表
    }

    playList->setCurrentIndex(0);
    player->setPlaylist(playList);
}

void Widget::on_player_Data_change()
{
    ui->list->setCurrentRow(playList->currentIndex());
    QString music_name = player->metaData(QMediaMetaData::Title).toString();        //歌曲名字
    QString author= player->metaData(QMediaMetaData::Author).toString();            //作者
    if(music_name == "" && author == "")
    {
        if(!ui->list->count() == 0)
        {
            ui->music_name_label->setText(ui->list->currentItem()->text());
        }
        else
            ui->music_name_label->setText("");
    }
    else
    {
        ui->music_name_label->setText(music_name+"-"+author);
    }


    int h = player->duration()/3600000;
    int m = (player->duration()%3600000)/60000;
    int s = ((player->duration()%3600000)%60000)/1000;
    int ms = ((player->duration()%3600000)%60000)%1000;
    QTime time(h,m,s,ms);     //音乐时长
    ui->time_label->setText(time.toString("hh:mm:ss"));
    ui->now_time_label->setText("00:00:00");

    ui->seekSlider->setMinimum(0);
    ui->seekSlider->setMaximum(player->duration());
    music_position = 0;

    if(playList->currentIndex() >= 0 && playList->currentIndex() < playList->mediaCount())
    {
        get_lrcStrTime();
    }
}
void Widget::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)            //播放完成自动跳转下一曲
{
    if(status == QMediaPlayer::EndOfMedia)
    {
        //播放完成
        on_next_Btn_clicked();
    }
}

void Widget::on_player_position_change(qint64 position)                         //播放歌曲进度改变，改变时间显示，歌词改变
{
    ui->seekSlider->setValue(position);
    music_position = position;

    int h = music_position/3600000;
    int m = (music_position%3600000)/60000;
    int s = ((music_position%3600000)%60000)/1000;
    int ms = ((music_position%3600000)%60000)%1000;
    QString timestr;
    QTime time(h,m,s,ms);
    ui->now_time_label->setText(time.toString("hh:mm:ss"));

    QTime time2(h,m,s,ms);
    timestr = time2.toString("mm:ss");

    for(int i = 0;i < lrcTime.count();i++)
    {
        if(time <= QTime::fromString(lrcTime.at(i),"mm:ss:zzz"))
        {
            // i 为下一次唱的歌词
            QString mm1;
            QString ss1;
            QString zzz1;
            QString mm2;
            QString ss2;
            QString zzz2;

            if(lrcindex != i || i == 0)
            {
                if(i == 0)
                {
                    myLrc_widget->setLabel_1_text(lrcStr.at(i));
                    myLrc_widget->setLabel_2_text(lrcStr.at(i + 1));
                }
                else
                {
                    if(nowIsFirst)
                    {
                        myLrc_widget->setLabel_2_text(lrcStr.at(i));
                        nowIsFirst = !nowIsFirst;
                    }
                    else
                    {
                        myLrc_widget->setLabel_1_text(lrcStr.at(i));
                        nowIsFirst = !nowIsFirst;
                    }
                }
                if(lrcindex <= 4)              //前5行   直接下来  歌词不变
                {
                    ui->Lrc_list->item(lrcindex)->setTextColor(Qt::red);
                    for(int j = 0;j < lrcindex;j++)
                    {
                        ui->Lrc_list->item(j)->setTextColor(Qt::black);
                    }
                }
                else            //超过5   通过改变每行显示的歌词来改变进度
                {
                    for(int j = 0;j < 4;j++)
                    {
                        ui->Lrc_list->item(j)->setText(lrcStr.at(lrcindex - 4 + j));
                    }
                    for(int j = 0;j < 6;j++)
                    {
                        if(lrcindex + j < lrcStr.count())
                        {
                            ui->Lrc_list->item(4+j)->setText(lrcStr.at(lrcindex + j));
                        }
                        else
                        {
                            ui->Lrc_list->item(4+j)->setText("");
                        }
                    }
                }
                lrcindex = i;
            }
            if(i != 0)
            {
                mm1 = QString(lrcTime.at(i)).split(":").at(0);
                ss1 = QString(lrcTime.at(i)).split(":").at(1);
                zzz1 = QString(lrcTime.at(i)).split(":").at(2);

                mm2 = QString(lrcTime.at(i - 1)).split(":").at(0);
                ss2 = QString(lrcTime.at(i - 1)).split(":").at(1);
                zzz2 = QString(lrcTime.at(i - 1)).split(":").at(2);
                int y = (mm1.toInt() - mm2.toInt())*60*1000 + (ss1.toInt() - ss2.toInt())*1000 + (zzz1.toInt() - zzz2.toInt());
                int x = (m - mm2.toInt())*60*1000 + (s - ss2.toInt())*1000 + (ms - zzz2.toInt());
                myLrc_widget->setProportion(x,y);
            }
            break;
        }
    }

}
void Widget::on_seekSlider_sliderReleased()     //歌曲进度条改变，播放位置相应改变
{
    music_position = ui->seekSlider->value();
    player->setPosition(music_position);
}

void Widget::on_playPattern_Btn_clicked()           //播放顺序
{
    playPattern++;
    playPattern = playPattern % 3;
    if(playPattern == 0)
    {
        ui->playPattern_Btn->setStyleSheet("QPushButton#playPattern_Btn{border-image: url(:/images/shunxu.png);}");
        playList->setPlaybackMode(QMediaPlaylist::Sequential);
        ui->playPattern_Btn->setToolTip(QString("顺序播放"));
    }
    else if(playPattern == 1)
    {
        ui->playPattern_Btn->setStyleSheet("QPushButton#playPattern_Btn{border-image: url(:/images/danqu.png);}");
        playList->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->playPattern_Btn->setToolTip(QString("单曲循环"));
    }
    else
    {
        ui->playPattern_Btn->setStyleSheet("QPushButton#playPattern_Btn{border-image: url(:/images/suiji.png);}");
        playList->setPlaybackMode(QMediaPlaylist::Random);
        ui->playPattern_Btn->setToolTip(QString("随机播放"));
    }
}

void Widget::on_next_Btn_clicked()          //下一曲
{
    player->setMedia(playList->media(playList->nextIndex()));
    playList->setCurrentIndex(playList->nextIndex());
    isPlay = false;
    on_play_Btn_clicked();
    if(playList->currentIndex() >= 0 && playList->currentIndex() < playList->mediaCount())
    {
        get_lrcStrTime();
    }
}

void Widget::on_last_Btn_clicked()          //上一曲
{
    player->setMedia(playList->media(playList->previousIndex()));
    playList->setCurrentIndex(playList->previousIndex());
    isPlay = false;
    on_play_Btn_clicked();
    if(playList->currentIndex() >= 0 && playList->currentIndex() < playList->mediaCount())
    {
        get_lrcStrTime();
    }
}

void Widget::on_search_Btn_clicked()        //点击搜索按钮，发送请求，得到歌曲和歌词
{
    if(!ui->search_LineEdit->text().isEmpty())
    {
        if(isPlay)
        {
            on_play_Btn_clicked();
        }
        player->setPlaylist(NULL);
        lrcList.clear();
        lrcStr.clear();
        lrcTime.clear();
        playList->clear();
        ui->list->clear();
        //发送歌曲搜索请求

        KuGouSearch->search(ui->search_LineEdit->text());
//        WyySearch->search(ui->search_LineEdit->text());
//        QqMusicSearch->search(ui->search_LineEdit->text());

        player->setPlaylist(playList);

        if(playList->currentIndex() >= 0 && playList->currentIndex() < playList->mediaCount())
        {
            get_lrcStrTime();
        }
    }
}

void Widget::get_lrcStrTime()       //得到歌词字符串，保存到list
{
    if(!playList->isEmpty() && !lrcList.isEmpty())
    {
        int index = playList->currentIndex();

        nowLrcListRow = 0;
        lrcTime.clear();
        lrcStr.clear();
        ui->Lrc_list->clear();
        QString str = lrcList.at(index);
        QStringList li = str.split("[");
        foreach (QString temp, li) {
            if(temp.contains("]"))
            {
                QString tempstr = temp.split("]").at(0);
//                qDebug()<<tempstr;
//                lrcTime.append(tempstr);
//                lrcTime.append(tempstr.split(".").at(0));
                QString ms = tempstr.split(".").at(1);
                ms = QString("%1").arg(ms.toInt(),3,10,QLatin1Char('0'));
                lrcTime.append(tempstr.split(".").at(0)+":"+ms);
                if(QString(temp.split("]").at(1)).contains("\n"))
                {
                    lrcStr.append(QString(temp.split("]").at(1)).remove("\n"));
                }
            }
        }
        if(lrcStr.count()>10)
        {
            for(int i = 0;i < 10;i++)
            {
                QListWidgetItem *add_item = new QListWidgetItem(ui->Lrc_list);
                add_item->setText(lrcStr.at(i));
                //设置item项中的文字位置
                add_item->setTextAlignment(Qt::AlignHCenter);
                add_item->setSizeHint(QSize(ui->Lrc_list->width(),ui->Lrc_list->height()/10));
                ui->Lrc_list->insertItem(i,add_item);
            }
        }
        else
        {
            for(int i = 0;i < lrcStr.count();i++)
            {
                QListWidgetItem *add_item = new QListWidgetItem(ui->Lrc_list);
                add_item->setText(lrcStr.at(i));
                //设置item项中的文字位置
                add_item->setTextAlignment(Qt::AlignHCenter);
                add_item->setSizeHint(QSize(ui->Lrc_list->width(),ui->Lrc_list->height()/10));
                ui->Lrc_list->insertItem(i,add_item);
            }
            for(int i = lrcStr.count();i < 10;i++)
            {
                QListWidgetItem *add_item = new QListWidgetItem(ui->Lrc_list);
                add_item->setText("");
                //设置item项中的文字位置
                add_item->setTextAlignment(Qt::AlignHCenter);
                add_item->setSizeHint(QSize(ui->Lrc_list->width(),ui->Lrc_list->height()/10));
                ui->Lrc_list->insertItem(i,add_item);
            }
        }
        lrcListFirstAdd = 0;
        nowLrcListRow = 0;
    }
}

void Widget::on_list_doubleClicked(const QModelIndex &index)        //双击列表，播放
{
    player->setMedia(playList->media(index.row()));
    playList->setCurrentIndex(index.row());
    isPlay = false;
    on_play_Btn_clicked();
    lrcindex = 0;
    if(playList->currentIndex() >= 0 && playList->currentIndex() < playList->mediaCount())
    {
        get_lrcStrTime();
    }

//    qDebug()<<ui->list->count();
//    qDebug()<<lrcList.count();
}

void Widget::listAdd(QString s)
{
    QListWidgetItem *add_item = new QListWidgetItem(QIcon(":/images/heart00.png"),s);
    ui->list->addItem(add_item);
}
void Widget::lrcStrAdd(QString s)
{
    lrcList.append(s);
}
void Widget::urlListAdd(QString s)
{
    playList->addMedia(QUrl(s));
}

void Widget::on_close_Btn_clicked()
{
    delete myLrc_widget;
    myLrc_widget = NULL;
    this->close();
}

void Widget::on_min_Btn_clicked()
{
    this->hide();

    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
}
void Widget::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        this->show();
        break;
    default:
        break;
    }
}
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(event->pos().y()<30)
        {
            mousePosLast = event->pos();
            lpressed = true;
        }
        else
        {
            lpressed = false;
        }
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(lpressed == true)
    {
        move(event->pos()-mousePosLast + pos());
    }
}
void Widget::keyPressEvent(QKeyEvent *event)            //按空格播放
{
    if(event->key() == Qt::Key_Space)
    {
        on_play_Btn_clicked();
    }
}
void Widget::doMoveTital()
{
    if(titalweizhi < titalStr.length())
    {
        QString temp = titalStr.mid(titalweizhi)+titalStr.mid(0,titalweizhi);
        ui->tital_label->setText(temp);
        titalweizhi++;
    }
    else
    {
        titalweizhi = 0;
    }
}
