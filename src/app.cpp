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

  QNetworkRequest request;
  request.setUrl(QUrl("https://api.openai.com/v1/chat/completions"));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  request.setRawHeader("Authorization", "Bearer YOUR-TOKEN-HERE");

  QString jsonBody = QString(R"({
        "model": "gpt-3.5-turbo",
        "messages": [{"role": "user", "content": "%1"}]
  })").arg(text);

  QNetworkReply *reply = manager->post(request, jsonBody.toUtf8());

  connect(reply, &QNetworkReply::finished, [=]() {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        if (!jsonDoc.isNull()) {
            QJsonObject jsonObj = jsonDoc.object();

            if (jsonObj.contains("choices") && jsonObj["choices"].isArray()) {
                QJsonArray choicesArray = jsonObj["choices"].toArray();

                if (!choicesArray.isEmpty()) {
                    QJsonValue choiceValue = choicesArray.at(0);

                    if (choiceValue.isObject()) {
                        QJsonObject choiceObj = choiceValue.toObject();

                        if (choiceObj.contains("message") && choiceObj["message"].isObject()) {
                            QJsonObject messageObj = choiceObj["message"].toObject();

                            if (messageObj.contains("content") && messageObj["content"].isString()) {
                              QString content = messageObj["content"].toString();

                              qDebug() << "Content:" << content;

                              std::cout << "IF!" << std::endl;

                              responseField->setProperty("text", content);

                              return;
                            }
                        }
                    }
                }
            }
        }
    } else {
      // Tratar erros
      qDebug() << reply->errorString();

      std::cout << "ELSE!" << std::endl;

      responseField->setProperty("text", reply->errorString());
    }
    reply->deleteLater();
    responseField->setProperty("placeholderText", "I am waiting for your question...");
  });
}
