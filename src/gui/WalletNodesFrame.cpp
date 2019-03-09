// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QApplication>
#include <QClipboard>
#include <QJsonObject>

#include "CurrencyAdapter.h"
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
  QString remoteNodeUrl = "localhost:8314/feeaddress";
  m_addressProvider->getAddress(remoteNodeUrl);
  connect(m_addressProvider, &AddressProvider::addressFoundSignal, this, &WalletNodesFrame::onAddressFound, Qt::QueuedConnection);
  remoteNodeUrl = "remote-nbr-001.niobioco.in:8314/feeaddress";
  m_addressProvider->getAddress(remoteNodeUrl);
  connect(m_addressProvider, &AddressProvider::addressFoundSignal, this, &WalletNodesFrame::onAddressFound, Qt::QueuedConnection);

}

WalletNodesFrame::~WalletNodesFrame() {
}

void WalletNodesFrame::onAddressFound(const QJsonObject& _remoteNodeData) {

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
    WalletNodesFrame::remote_node_fee_percent = feePercent;
    qDebug()<< "url " << url << " - feePercent " << feePercent << "\n";
  } else {
    qDebug()<< "feePercent not found\n";
    WalletNodesFrame::remote_node_fee_percent = 0;
  }
}

}
