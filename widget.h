#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QMediaPlaylist>
#include <QMediaPlayer>
#include<QPaintEvent>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include<QJsonValue>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include"kugou.h"
#include<QSystemTrayIcon>
#include<QMouseEvent>
#include<QKeyEvent>
#include<QTimer>
#include"lrc_widget.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void setBtnStyle();
    void parseJson_getAlbumID(QString json);
    void parseJson_getplay_url(QString json);
    void get_lrcStrTime();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private slots:
    void on_play_Btn_clicked();

    void on_muteButton_clicked();

    void on_volumeControl_valueChanged(int value);

    void on_add_Btn_clicked();

    void on_player_Data_change();

    void on_player_position_change(qint64);

    void on_seekSlider_sliderReleased();

    void on_playPattern_Btn_clicked();

    void on_next_Btn_clicked();

    void on_last_Btn_clicked();

    void paintEvent(QPaintEvent *event);
    void on_search_Btn_clicked();

    void on_list_doubleClicked(const QModelIndex &index);
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);

    void listAdd(QString s);
    void lrcStrAdd(QString s);
    void urlListAdd(QString s);
    void on_close_Btn_clicked();

    void on_min_Btn_clicked();
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void doMoveTital();
private:
    Ui::Widget *ui;
    bool isPlay;
    bool isMute;

    QString searchstr;
    QString hashStr;
    QString album_idStr;
    int music_position;
    int music_volume;

    int playPattern;
    QMediaPlayer* player;
    QMediaPlaylist* playList;

    QStringList lrcTime;
    QStringList lrcStr;
    QStringList lrcList;
    bool nowIsFirst;

    QPoint mousePosLast;

    KuGou* KuGouSearch;
    QSystemTrayIcon *mSysTrayIcon;
    bool lpressed;

    int nowLrcListRow;
    int lrcListFirstAdd;
    QString titalStr;
    int titalweizhi;

    int lrcindex;
    lrc_widget* myLrc_widget;
};

#endif // WIDGET_H
