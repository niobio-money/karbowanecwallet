// Copyright (c) 2019 Niobio developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "WalletNodes.h"
#include "Settings.h"
#include <iostream>

using namespace WalletGui;

WalletNodes::WalletNodes(QObject *parent) :
    QObject(parent)
    , myNetManager(new QNetworkAccessManager(this))
    , myNetReply(nullptr)
{
}

WalletNodes::~WalletNodes()
{
}

void WalletNodes::GetWalletNodes()
{
  if(myNetManager->networkAccessible() == QNetworkAccessManager::Accessible)
  {
    const QUrl WALLETNODES_LIST_URL("https://raw.githubusercontent.com/niobio-cash/wallet-support-files/master/wallet-nodes.txt");
    myNetReply = myNetManager->get(QNetworkRequest(WALLETNODES_LIST_URL));
    connect(myNetReply, &QNetworkReply::finished, this, &WalletNodes::OnListReadFinished);
  }
}

void WalletNodes::OnListReadFinished()
{
  QString result = myNetReply->readAll().data();
  result = result.trimmed();
  listOfNodes = result.split("\n");
  QStringList currentRpcNodesList = Settings::instance().getRpcNodesList();
  Q_FOREACH (const QString& node, listOfNodes) {
    if (!currentRpcNodesList.contains(node)) {
      currentRpcNodesList << node;
    }
  }
  Settings::instance().setRpcNodesList(currentRpcNodesList);
	NetworkCleanup();
}

void WalletNodes::NetworkCleanup()
{
	myNetReply->deleteLater();
	myNetReply = nullptr;
}
