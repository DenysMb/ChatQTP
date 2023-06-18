// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: %{CURRENT_YEAR} %{AUTHOR} <%{EMAIL}>

#include "app.h"
#include <KSharedConfig>
#include <KWindowConfig>
#include <QQuickWindow>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

#define CURL_STATICLIB

void App::restoreWindowGeometry(QQuickWindow *window, const QString &group) const
{
  KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
  KConfigGroup windowGroup(&dataResource, QStringLiteral("Window-") + group);
  KWindowConfig::restoreWindowSize(window, windowGroup);
  KWindowConfig::restoreWindowPosition(window, windowGroup);
}

void App::saveWindowGeometry(QQuickWindow *window, const QString &group) const
{
  KConfig dataResource(QStringLiteral("data"), KConfig::SimpleConfig, QStandardPaths::AppDataLocation);
  KConfigGroup windowGroup(&dataResource, QStringLiteral("Window-") + group);
  KWindowConfig::saveWindowPosition(window, windowGroup);
  KWindowConfig::saveWindowSize(window, windowGroup);
  dataResource.sync();
}

void App::request(QObject *responseField, const QString& text) const
{
  responseField->setProperty("placeholderText", "Wait a moment, I am thinking...");

  QNetworkAccessManager *manager = new QNetworkAccessManager();

    QUrl url("https://chatbot.theb.ai/api/chat-process");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/114.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString jsonData = QString("{\"prompt\":\"%1\",\"options\":{}}").arg(text);

    QNetworkReply *reply = manager->post(request, jsonData.toUtf8());

    // Connect the signals and slots to handle the response
    connect(reply, &QNetworkReply::readyRead, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QString response = QString::fromUtf8(data);

            QStringList objects = response.split('\n');
            QString lastObject = objects.last();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(lastObject.toUtf8());
            QJsonObject jsonObj = jsonDoc.object();
            QString textValue = jsonObj.value("text").toString();

            // Update the responseField with the received data
            responseField->setProperty("text", textValue);

            return;
        } else {
            // Handle the error case
            qDebug() << "Error: " << reply->errorString();

            responseField->setProperty("text", reply->errorString());
        }

        // Clean up the reply and manager
        reply->deleteLater();
        manager->deleteLater();

        responseField->setProperty("placeholderText", "I am waiting for your question...");
    });
}
