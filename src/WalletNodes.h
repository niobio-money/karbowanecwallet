// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#pragma once
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

class WalletNodes : public QObject
{
  Q_OBJECT

public:
  WalletNodes(QObject * parent = nullptr);
  ~WalletNodes();
  void GetWalletNodes();
  QStringList listOfNodes;

private:
  void NetworkCleanup();

signals:

private slots:
  void OnListReadFinished();

private:
  QNetworkAccessManager * myNetManager;
  QNetworkReply * myNetReply;
  void saveSettings(const QStringList& _RpcNodesList);
};
