#include "plugins.h"

#include "util/request.h"
#include "util/option.h"

#include <QByteArray>
#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>

const static auto& request = QCloudMusicApi::Request::createRequest;

using namespace QCloudMusicApi;
Plugins::Plugins(QObject* parent)
    : QObject{ parent }
{}

QVariantMap Plugins::songUpload(QVariantMap query)
{
    QString ext = "mp3";
    if (query.value("songFile").toMap()["name"].toString().toLower().indexOf("flac") > -1) {
        ext = "flac";
    }
    QString filename = query.value("songFile").toMap()["name"].toString()
        .replace("." + ext, "")
        .replace(QRegularExpression("\\s"), "")
        .replace(QRegularExpression("\\."), "_");
    const QString bucket = "jd-musicrep-privatecloud-audio-public";
    //   获取key和token
    const auto tokenRes = request(
        "/api/nos/token/alloc",
        {
            { "bucket", bucket },
            { "ext", ext },
            { "filename", filename },
            { "local", false },
            { "nos_product", 3 },
            { "type", "audio" },
            { "md5", query["songFile"].toMap()["md5"] },
        },
        Option::createOption(query, "weapi")
        );

    const auto tokenBody = tokenRes["body"].toMap();
    QVariantMap uploadInfo{
        { "token", tokenBody }
    };

    const auto tokenCode = tokenBody.value("code").toInt();

    if (tokenRes["status"].toInt() >= 400 || tokenBody.isEmpty() || tokenCode != 200) {
        const auto message = tokenBody.value("message").toString().isEmpty()
            ? tokenBody.value("msg").toString()
            : tokenBody.value("message").toString();
        uploadInfo["error"] = QVariantMap{
            { "stage", "token" },
            { "code", tokenCode != 200 ? tokenCode : tokenRes["status"].toInt() },
            { "msg", message.isEmpty() ? QStringLiteral("token alloc failed") : message }
        };
        return uploadInfo;
    }

    const auto tokenResult = tokenBody.value("result").toMap();
    const auto objectKeyRaw = tokenResult.value("objectKey").toString();
    const auto tokenValue = tokenResult.value("token").toString();
    if (objectKeyRaw.isEmpty() || tokenValue.isEmpty()) {
        uploadInfo["error"] = QVariantMap{
            { "stage", "token" },
            { "code", -1 },
            { "msg", "token alloc response missing required fields" }
        };
        return uploadInfo;
    }

    // 上传
    auto objectKey = objectKeyRaw;
    objectKey.replace("/", "%2F");

    auto reply = Request::axios(
        QNetworkAccessManager::GetOperation,
        "https://wanproxy.127.net/lbs?version=1.0&bucketname=" + bucket,
        {}, {}, "");
    const auto lbsError = reply->error();
    const auto lbsErrorMessage = reply->errorString();
    const auto lbsStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const auto lbsRaw = reply->readAll();
    reply->manager()->deleteLater();
    if (lbsError != QNetworkReply::NoError) {
        qDebug().noquote() << "songUpload: failed to fetch upload host" << lbsErrorMessage;
        uploadInfo["error"] = QVariantMap{
            { "code", static_cast<int>(lbsError) },
            { "statusCode", lbsStatusCode },
            { "msg", "lbs: " + lbsErrorMessage }
        };
        return uploadInfo;
    }

    const auto lbsVariant = QJsonDocument::fromJson(lbsRaw).toVariant();
    if (lbsVariant.isNull()) {
        qDebug().noquote() << "songUpload: invalid lbs response";
        uploadInfo["error"] = QVariantMap{
            { "code", lbsStatusCode },
            { "msg", "lbs: invalid lbs response" }
        };
        uploadInfo["lbsRaw"] = QString::fromUtf8(lbsRaw);
        return uploadInfo;
    }

    const auto lbs = lbsVariant.toMap();
    uploadInfo["lbs"] = lbs;

    const auto uploadHosts = lbs.value("upload").toList();
    if (uploadHosts.isEmpty() || uploadHosts.first().toString().isEmpty()) {
        qDebug().noquote() << "songUpload: upload host missing in lbs response";
        uploadInfo["error"] = QVariantMap{
            { "code", lbsStatusCode },
            { "msg", "lbs: upload host missing" }
        };
        return uploadInfo;
    }
    const auto uploadHost = uploadHosts.first().toString();

    reply = Request::axios(
        QNetworkAccessManager::PostOperation,
        uploadHost + "/" + bucket + "/" + objectKey + "?offset=0&complete=true&version=1.0",
        {},
        {
            { "x-nos-token", tokenValue },
            { "Content-MD5", query["songFile"].toMap()["md5"] },
            { "Content-Type", "audio/mpeg" },
            { "Content-Length", query["songFile"].toMap()["size"] },
        },
        query["songFile"].toMap()["data"].toByteArray());
    const auto uploadError = reply->error();
    const auto uploadErrorMessage = reply->errorString();
    const auto uploadStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const auto uploadRaw = reply->readAll();
    reply->manager()->deleteLater();

    QVariant uploadPayload;
    const auto uploadDoc = QJsonDocument::fromJson(uploadRaw);
    if (!uploadDoc.isNull() && uploadDoc.isObject()) {
        uploadPayload = uploadDoc.toVariant();
    }
    else if (!uploadRaw.isEmpty()) {
        uploadPayload = QString::fromUtf8(uploadRaw);
    }
    if (!uploadPayload.isNull()) {
        uploadInfo["upload"] = uploadPayload;
    }

    if (uploadError != QNetworkReply::NoError) {
        qDebug().noquote() << "songUpload: upload request failed" << uploadErrorMessage;
        uploadInfo["error"] = QVariantMap{
            { "stage", "upload" },
            { "code", static_cast<int>(uploadError) },
            { "statusCode", uploadStatusCode },
            { "msg", uploadErrorMessage }
        };
        return uploadInfo;
    }

    uploadInfo["code"] = uploadStatusCode;
    uploadInfo["success"] = true;
    return uploadInfo;
}

QVariantMap Plugins::upload(QVariantMap query)
{
    const QVariantMap data{
        { "bucket", "yyimgs" },
        { "ext", "jpg" },
        { "filename", query["imgFile"].toMap()["name"] },
        { "local", false },
        { "nos_product", 0 },
        { "return_body", "{\"code\":200,\"size\":\"$(ObjectSize)\"}" },
        { "type", "other" }
    };
    //   获取key和token
    const auto res = request(
        "/api/nos/token/alloc",
        data,
        Option::createOption(query, "weapi")
    );
    auto reply = Request::axios(QNetworkAccessManager::PostOperation,
        "https://nosup-hz1.127.net/yyimgs/"
        + res["body"].toMap()["result"].toMap()["objectKey"].toString()
        + "?offset=0&complete=true&version=1.0",
        {},
                                {
                                    { "x-nos-token", res["body"].toMap()["result"].toMap()["token"] },
                                    { "Content-Type", "image/jpeg" }
                                },
        query["imgFile"].toMap()["data"].toByteArray());
    reply->manager()->deleteLater();

    // 读取响应内容
    auto body = reply->readAll();
    qDebug().noquote() << body.isEmpty() << "body" << body;

    QVariantMap res2 = QJsonDocument::fromJson(body).toVariant().toMap();

    return {
        // ...res.body.result,
        // ...res2.data,
        // ...res3.body,
        { "url_pre", "https://p1.music.126.net/" + res["body"].toMap()["result"].toMap()["objectKey"].toString() },
        { "imgId", res["body"].toMap()["result"].toMap()["id"] },
    };
}
