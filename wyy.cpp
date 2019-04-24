#include "wyy.h"

WYY::WYY(QObject *parent):QObject(parent)
{
    network_manager = new QNetworkAccessManager();
    network_request = new QNetworkRequest();
    network_manager2 = new QNetworkAccessManager();
    network_request2 = new QNetworkRequest();
    network_manager3 = new QNetworkAccessManager();
    network_request3 = new QNetworkRequest();
    connect(network_manager3, &QNetworkAccessManager::finished, this, &WYY::replyFinished3);
    connect(network_manager2, &QNetworkAccessManager::finished, this, &WYY::replyFinished2);
    connect(network_manager, &QNetworkAccessManager::finished, this, &WYY::replyFinished);
}
void WYY::replyFinished(QNetworkReply *reply)        //发送搜索请求完成，接受到信息，然后进行解析
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
void WYY::replyFinished3(QNetworkReply *reply)        //发送搜索请求完成，接受到信息，然后进行解析
{
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串
        parseJson_getlrc(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        //处理错误
        qDebug()<<"处理错误";
    }
}
void WYY::replyFinished2(QNetworkReply *reply)       //发送搜索请求完成，接受到信息，然后进行解析         通过歌曲ID搜索
{
    //获取响应的信息，状态码为200表示正常
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  //获取字节
        QString result(bytes);  //转化为字符串

        parseJson_getplay_url(result);  //自定义方法，解析歌曲数据
    }
    else
    {
        //处理错误
        qDebug()<<"处理错误";
    }
}
void WYY::parseJson_getAlbumID(QString json)     //解析接收到的歌曲信息，得到歌曲ID
{
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            if(rootObj.contains("result"))
            {
                QJsonValue valuedata = rootObj.value("result");
                if(valuedata.isObject())
                {
                    QJsonObject valuedataObject = valuedata.toObject();
                    if(valuedataObject.contains("songs"))
                    {
                        QJsonValue valueArray = valuedataObject.value("songs");
                        if (valueArray.isArray())
                        {
                            QJsonArray array = valueArray.toArray();
                            int size = array.size();
                            for(int i = 0;i < size;i++)
                            {
                                QJsonValue value = array.at(i);
                                if(value.isObject())
                                {
                                    QJsonObject object = value.toObject();
                                    if(object.contains("id"))
                                    {
                                        QJsonValue songsid_value = object.take("id");
                                        if(songsid_value.isString())
                                        {
                                            songsid = songsid_value.toString();             //歌曲ID信息
                                        }

                                    }
                                    if(object.contains("name"))
                                    {
                                        QJsonValue name_value = object.take("name");
                                        if(name_value.isString())
                                        {
                                            name = name_value.toString();             //歌曲name信息
                                        }
                                    }
                                    if(object.contains("artists"))      //作者
                                    {
                                        QJsonValue valueArray2 = object.value("artists");
                                        if(valueArray2.isArray())
                                        {
                                            QJsonValue value2 = valuedataObject.value("artists").toArray().at(0);
                                            if(value2.isObject())
                                            {
                                                QJsonObject object2 = value2.toObject();
                                                if(object2.contains("name"))
                                                {
                                                    QJsonValue name_value = object.take("name");
                                                    if(name_value.isString())
                                                    {
                                                        name = name + "-" + name_value.toString();             //歌曲name信息
                                                        emit nameAdd(name);
                                                    }
                                                }
                                            }
                                        }
                                    }
//                                    if(object.contains("FileHash"))
//                                    {
//                                        QJsonValue FileHash_value = object.take("FileHash");
//                                        if(FileHash_value.isString())
//                                        {
//                                            hashStr = FileHash_value.toString();                //hash
//                                        }
//                                    }

                                    //通过歌曲ID发送请求，得到歌曲url和歌词
                                    QString WYYAPISTR1 = QString("http://musicapi.leanapp.cn/music/url?id=%1")
                                            .arg(songsid);
                                    QString WYYAPISTR2 = QString("http://musicapi.leanapp.cn/lyric?id=%1").arg(songsid);
                                    network_request2->setUrl(QUrl(WYYAPISTR1));
                                    network_manager2->get(*network_request2);

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

void WYY::parseJson_getplay_url(QString json)        //解析得到歌曲
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
                if(valuedata.isArray())
                {
                    QJsonArray array = valuedata.toArray();
                    int size = array.size();
                    for(int i = 0;i < size;i++)
                    {
                        QJsonValue value = array.at(i);
                        if(value.isObject())
                        {
                            QJsonObject object = value.toObject();
                            if(object.contains("url"))
                            {
                                QJsonValue url_value = object.take("url");
                                if(url_value.isString())
                                {
                                    QString url = url_value.toString();             //歌曲url
                                    emit mediaAdd(url);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void WYY::parseJson_getlrc(QString json)        //解析得到歌曲lrc
{
    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(json), &json_error);
    if(json_error.error == QJsonParseError::NoError)
    {
        if(parse_doucment.isObject())
        {
            QJsonObject rootObj = parse_doucment.object();
            if(rootObj.contains("lrc"))
            {
                QJsonValue valuedata = rootObj.value("lrc");
                if(valuedata.isObject())
                {
                    QJsonObject obj2 = valuedata.toObject();
                    if(obj2.contains("lyric"))
                    {
                        QJsonValue lrc_value = obj2.take("lyric");
                        if(lrc_value.isString())
                        {
                            QString lrc = lrc_value.toString();             //歌曲url
                            emit lrcAdd(lrc);
                        }
                    }
                }
            }
        }
    }
}
void WYY::search(QString str)
{
    //发送歌曲搜索请求
    QString KGAPISTR1 = QString("http://musicapi.leanapp.cn/search?keywords=%1").arg(str);
    qDebug()<<"KGAPISTR1:"<<KGAPISTR1;
    network_request->setUrl(QUrl(KGAPISTR1));
    network_manager->get(*network_request);
}
