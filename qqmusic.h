#ifndef QQMUSIC_H
#define QQMUSIC_H

#include <QObject>
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

class QQMusic : public QObject
{
    Q_OBJECT
public:
    explicit QQMusic(QObject *parent = 0);
    void parseJson_getAlbumID(QString json);
    void parseJson_getplay_url(QString json);
    void parseJson_getlrc(QString json);

    void search(QString str);
signals:
    void nameAdd(QString name);
    void mediaAdd(QString urlStr);
    void lrcAdd(QString lrcStr);
public slots:
    void replyFinished(QNetworkReply *reply);
//    void replyFinished2(QNetworkReply *reply);
//    void replyFinished3(QNetworkReply *reply);
private:
    QNetworkAccessManager* network_manager;
    QNetworkRequest* network_request;
    QNetworkAccessManager* network_manager2;
    QNetworkRequest* network_request2;
    QNetworkAccessManager* network_manager3;
    QNetworkRequest* network_request3;

    QString hashStr;
    QString album_idStr;

    QString songsid;
    QString name;
};

#endif // QQMUSIC_H
