// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "WalletAdapter.h"
#include "WalletNodesModel.h"
#include "Settings.h"
#include "WalletNodes.h"
#include <iostream>

namespace WalletGui {

WalletNodesModel& WalletNodesModel::instance() {
  static WalletNodesModel inst;
  return inst;
}

WalletNodesModel::WalletNodesModel() : QAbstractTableModel() {
  connect(&WalletAdapter::instance(), &WalletAdapter::walletInitCompletedSignal, this, &WalletNodesModel::walletInitCompleted, Qt::QueuedConnection);
  connect(&WalletAdapter::instance(), &WalletAdapter::walletCloseCompletedSignal, this, &WalletNodesModel::reset, Qt::QueuedConnection);
}

WalletNodesModel::~WalletNodesModel() {
}

int WalletNodesModel::columnCount(const QModelIndex& _parent) const {
  return 2;
}

QVariant WalletNodesModel::data(const QModelIndex& _index, int _role) const {
  if (!_index.isValid()) {
    return QVariant();
  }

  QJsonObject wallenode = m_walletNodes.at(_index.row()).toObject();

  switch (_role) {
  case Qt::DisplayRole:
    switch (_index.column()) {
    case COLUMN_URL:
      return _index.data(ROLE_URL);
    case COLUMN_FEE:
      return _index.data(ROLE_FEE);
    default:
      return QVariant();
    }

  case ROLE_URL:
    return wallenode.value("url");
  case ROLE_FEE:
    return wallenode.value("fee");
  default:
    return QVariant();
  }

  return QVariant();
  /*
  if (_role == Qt::DisplayRole) {
    if (_index.column() == 0) {
      return m_walletNodes.at(_index.row()).toString();
    } else if (_index.column() == 1) {
      return m_walletNodesFees.at(_index.row()).toDouble();
    } else if (_index.column() > 1) {
      return QString("-");
    }
  }
  return QVariant();
  */
}

Qt::ItemFlags WalletNodesModel::flags(const QModelIndex& _index) const {
  return (Qt::ItemIsEnabled | Qt::ItemNeverHasChildren | Qt::ItemIsSelectable);
}

QVariant WalletNodesModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
  if (_orientation != Qt::Horizontal || _role != Qt::DisplayRole) {
    return QVariant();
  }

  switch (_section) {
  case COLUMN_URL:
    return tr("URL");
  case COLUMN_FEE:
    return tr("Fee (%)");
  }

  return QVariant();
  /*
  if (_role == Qt::DisplayRole && _orientation == Qt::Horizontal) {
      switch (_section) {
      case 0:
          return QString("URL");
      case 1:
          return QString("Fee (%)");
      }
  }
  return QVariant();
  */
}

QModelIndex WalletNodesModel::index(int _row, int _column, const QModelIndex& _parent) const {
  if (_parent.isValid()) {
    return QModelIndex();
  }

  return createIndex(_row, _column, _row);
}

QModelIndex WalletNodesModel::parent(const QModelIndex& _index) const {
  return QModelIndex();
}

int WalletNodesModel::rowCount(const QModelIndex& _parent) const {
  return m_walletNodes.size();
}

void WalletNodesModel::addWalletNode(const QString& _url, const QString& _fee) {
  beginInsertRows(QModelIndex(), m_walletNodes.size(), m_walletNodes.size());
  QJsonObject newWalletNode;
  newWalletNode.insert("url", _url);
  newWalletNode.insert("fee", _fee);
  m_walletNodes.append(newWalletNode);
  endInsertRows();
  saveWalletNodes();
}

void WalletNodesModel::removeWalletNode(quint32 _row) {
  if (_row > m_walletNodes.size() - 1) {
    return;
  }

  beginRemoveRows(QModelIndex(), _row, _row);
  m_walletNodes.removeAt(_row);
  endRemoveRows();
  saveWalletNodes();
}

void WalletNodesModel::reset() {
  beginResetModel();
  while (!m_walletNodes.empty()) {
    m_walletNodes.removeFirst();
  }

  endResetModel();
}

void WalletNodesModel::saveWalletNodes() {
  /*
  QFile addressBookFile(Settings::instance().getAddressBookFile());
  if (addressBookFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    QByteArray file_content = QJsonDocument(m_walletNodes).toJson(QJsonDocument::Compact);
    addressBookFile.write(file_content);
    addressBookFile.close();
  }
  */
}

void WalletNodesModel::walletInitCompleted(int _error, const QString& _error_text) {
  if (!_error) {
    QFile walletNodesFile(Settings::instance().getWalletNodesFile());
    if (walletNodesFile.open(QIODevice::ReadOnly)) {
      //QByteArray file_content = walletNodesFile.readAll();
      //QJsonDocument doc = QJsonDocument::fromJson(file_content);
      QJsonObject obj = QJsonDocument::fromJson(walletNodesFile.readAll()).object();
      if (obj.contains("remoteNodes")) {
      //if (!doc.isNull()) {
        m_walletNodes = obj.value("remoteNodes").toArray();
      }

      walletNodesFile.close();
      if (!m_walletNodes.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, m_walletNodes.size() - 1);
        endInsertRows();
      }
    }
    /*
    QJsonValue currentRpcNodesList = Settings::instance().getRpcNodesListAsJson();
    m_walletNodes = currentRpcNodesList.toArray();
    if (!m_walletNodes.isEmpty()) {
      beginInsertRows(QModelIndex(), 0, m_walletNodes.size() - 1);
      endInsertRows();
    }
    Q_FOREACH (const QJsonValue& node, m_walletNodes) {
      m_walletNodesFees.append(QJsonValue(3.5));
    }
    */
  }
}

const QModelIndex WalletNodesModel::indexFromUrl(const QString& searchstring, const int& column){
  QModelIndex index = match(WalletNodesModel::index(0,column,QModelIndex()),
  Qt::DisplayRole, searchstring, 1,
  Qt::MatchFlags(Qt::MatchExactly|Qt::MatchRecursive))
  .value(0);

  return index;
}
}
