// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSortFilterProxyModel>

#include "CurrencyAdapter.h"
#include "WalletAdapter.h"
#include "WalletNodesModel.h"
#include "WalletNodesFrame.h"
#include "MainWindow.h"
#include "NewNodeDialog.h"
#include "WalletEvents.h"
#include "ui_walletnodesframe.h"
#include "Settings.h"
#include "AddressProvider.h"

namespace WalletGui {

  WalletNodesFrame::WalletNodesFrame(QWidget* _parent) : QFrame(_parent), m_ui(new Ui::WalletNodesFrame), m_addressProvider(new AddressProvider(this)) {
    m_ui->setupUi(this);
    m_ui->m_walletNodesView->setModel(&WalletNodesModel::instance());
    m_ui->m_walletNodesView->header()->setStretchLastSection(true);
    m_ui->m_walletNodesView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_ui->m_walletNodesView->setSortingEnabled(true);
    m_ui->m_walletNodesView->sortByColumn(1, Qt::AscendingOrder);
    m_ui->m_walletNodesView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(&WalletAdapter::instance(), &WalletAdapter::walletInitCompletedSignal, this, &WalletNodesFrame::walletInitCompleted, Qt::QueuedConnection);
  }

  WalletNodesFrame::~WalletNodesFrame() {
  }

  void WalletNodesFrame::setClicked() {
    QModelIndexList selection = m_ui->m_walletNodesView->selectionModel()->selectedRows();
    QModelIndex index = selection.at(0);
    QString url = index.sibling(index.row(),index.column()).data().toString();
    Settings::instance().setCurrentRemoteNode(url);
    Settings::instance().setConnection("remote");
    QCoreApplication::postEvent(&MainWindow::instance(), new ShowMessageEvent(tr("Change will take effect after restarting the wallet."), QtInfoMsg));
  }

  void WalletNodesFrame::onAddressFound(const QJsonObject& _remoteNodeData) {
    QString currentWalletNode = Settings::instance().getCurrentRemoteNode();
    QString url = _remoteNodeData.value("url").toString();
    QString address = _remoteNodeData.value("fee_address").toString();
    if (!address.isEmpty()) {
      float feePercent = 0.25;
      if(_remoteNodeData.contains("fee_percent")) {
        feePercent = _remoteNodeData.value("fee_percent").toDouble();
        if (feePercent < 0 || feePercent > 5) { // prevent abuse
          feePercent = 0.25;
        }
      }
      QAbstractItemModel *modl = m_ui->m_walletNodesView->model();
      QSortFilterProxyModel proxy;
      proxy.setSourceModel(modl);
      proxy.setFilterKeyColumn(0);
      proxy.setFilterFixedString(url);
      QModelIndex matchingIndex = proxy.mapToSource(proxy.index(0,0));
      if(matchingIndex.isValid()) {
        modl->setData(matchingIndex, feePercent);
        if(url == currentWalletNode) {
          m_ui->m_walletNodesView->selectionModel()->setCurrentIndex(matchingIndex, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
      }
    }
  }

  void WalletNodesFrame::walletInitCompleted(int _error, const QString& _error_text) {
    QFile walletNodesFile(Settings::instance().getWalletNodesFile());
    if (walletNodesFile.open(QIODevice::ReadOnly)) {
      QJsonArray m_walletNodes;
      QJsonObject obj = QJsonDocument::fromJson(walletNodesFile.readAll()).object();
      if (obj.contains("remoteNodes")) {
        m_walletNodes = obj.value("remoteNodes").toArray();
      }
      walletNodesFile.close();
      if (!m_walletNodes.isEmpty()) {
        QString remoteNodeUrl;
        Q_FOREACH (const QJsonValue& node, m_walletNodes) {
          remoteNodeUrl = node.toObject().value("url").toString() + "/feeaddress";
          m_addressProvider->getAddress(remoteNodeUrl);
          connect(m_addressProvider, &AddressProvider::addressFoundSignal, this, &WalletNodesFrame::onAddressFound, Qt::QueuedConnection);
        }
      }
    }
  }
}
