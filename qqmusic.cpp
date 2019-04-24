#include "qqmusic.h"

QQMusic::QQMusic(QObject *parent) : QObject(parent)
{
    network_manager = new QNetworkAccessManager();
    network_request = new QNetworkRequest();
    network_manager2 = new QNetworkAccessManager();
    network_request2 = new QNetworkRequest();
    network_manager3 = new QNetworkAccessManager();
    network_request3 = new QNetworkRequest();
//    connect(network_manager3, &QNetworkAccessManager::finished, this, &QQMusic::replyFinished3);
//    connect(network_manager2, &QNetworkAccessManager::finished, this, &QQMusic::replyFinished2);
    connect(network_manager, &QNetworkAccessManager::finished, this, &QQMusic::replyFinished);
}
void QQMusic::search(QString str)
{
    //发送歌曲搜索请求
    QString KGAPISTR1 = QString("http://59.37.96.220/soso/fcgi-bin/client_search_cp?format=json&t=0&inCharset=GB2312&outCharset=utf-8&qqmusic_ver=1302&catZhida=0&p=1"
                                "&n=20&w=%1&flag_qc=0&remoteplace=sizer.newclient.song&new_json=1&lossless=0&aggr=1&cr=1&sem=0&force_zonghe=0").arg(str);
    qDebug()<<"KGAPISTR1:"<<KGAPISTR1;
    network_request->setUrl(QUrl(KGAPISTR1));
    network_manager->get(*network_request);
}
void QQMusic::replyFinished(QNetworkReply *reply)        //发送搜索请求完成，接受到信息，然后进行解析
{
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	
    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        parseJson_getAlbumID(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        //处理错误
        qDebug()<<"处理错误";
    }
}
void QQMusic::parseJson_getAlbumID(QString json)     //解析接收到的歌曲信息，得到歌曲ID
{
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            if(rootObj.contains("data"))
            {
                QJsonValue valuedata = rootObj.value("data");
                if(valuedata.isObject())
                {
                    QJsonObject valuedataObject = valuedata.toObject();
                    if(valuedataObject.contains("song"))
                    {
                        QJsonValue valuesong = valuedataObject.value("song");
                        if (valuesong.isObject())
                        {
                            QJsonObject valuelist = valuesong.toObject();
                            if(valuelist.contains("list"))
                            {
                                QJsonValue valueArray = valuelist.value("list");
                                if(valueArray.isArray())
                                {
                                    QJsonArray array = valueArray.toArray();
                                    int size = array.size();
                                    for(int i = 0;i < size;i++)
                                    {
                                        QJsonValue value = array.at(i);
                                        if(value.isObject())
                                        {
                                            QJsonObject object = value.toObject();
                                            if(object.contains("mid"))
                                            {
                                                QJsonValue AlbumID_value = object.take("mid");
                                                if(AlbumID_value.isString())
                                                {
                                                    album_idStr = AlbumID_value.toString();             //歌曲ID信息
                                                    qDebug()<<"url:"<<QString("http://ws.stream.qqmusic.qq.com/C100%1.m4a?fromtag=0&guid=126548448")
                                                              .arg(album_idStr);
                                                    emit mediaAdd(QString("http://ws.stream.qqmusic.qq.com/C100%1.m4a?fromtag=0&guid=126548448")
                                                                  .arg(album_idStr));
                                                }
                                            }
                                            if(object.contains("name"))
                                            {
                                                QJsonValue songname_value = object.take("name");
                                                if(songname_value.isString())
                                                {
                                                    name = songname_value.toString();             //歌曲名字
                                                }
                                            }
                                            if(object.contains("singer"))
                                            {
                                                QJsonValue singer_value = object.take("singer");
                                                if(singer_value.isArray())
                                                {
                                                    QJsonValue singername_value = singer_value.toArray().at(0);
                                                    if(singername_value.isObject())
                                                    {
                                                        QJsonObject singername_object = singername_value.toObject();
                                                        if(singername_object.contains("name"))
                                                        {
                                                            QJsonValue singer_name_value = singername_object.take("name");
                                                            if(singer_name_value.isString())
                                                            {
                                                                name = name + "-" + singer_name_value.toString();             //歌曲ID信息
                                                                emit nameAdd(name);
                                                            }
                                                        }
                                                    }

                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        qDebug()<<json_error.errorString();
    }
}
