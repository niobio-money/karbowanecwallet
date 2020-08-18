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
    return 4;
  }

  QVariant WalletNodesModel::data(const QModelIndex& _index, int _role) const {
    if (!_index.isValid()) {
      return QVariant();
    }

    QJsonObject walletNode = m_walletNodes.at(_index.row()).toObject();

    switch (_role) {
      case Qt::DisplayRole:
      switch (_index.column()) {
        case COLUMN_URL:
        return _index.data(ROLE_URL);
        case COLUMN_FEE:
        return _index.data(ROLE_FEE);
        case COLUMN_LATENCY:
        return _index.data(ROLE_LATENCY);
        case COLUMN_MESSAGE:
        return _index.data(ROLE_MESSAGE);
        default:
        return QVariant();
      }

      case ROLE_URL:
      return walletNode.value("url");
      case ROLE_FEE:
      return walletNode.value("fee");
      case ROLE_LATENCY:
      return walletNode.value("latency");
      case ROLE_MESSAGE:
      return walletNode.value("message");
      default:
      return QVariant();
    }
    return QVariant();
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
      case COLUMN_LATENCY:
      return tr("Latency (ms)");
      case COLUMN_MESSAGE:
      return tr("Message");
    }

    return QVariant();
  }

  int WalletNodesModel::rowCount(const QModelIndex& _parent) const {
    return m_walletNodes.size();
  }

  void WalletNodesModel::addWalletNode(const QModelIndex& _index, const QVariant& _value) {
    QVector<QString> walletNodeData = _value.value<QVector<QString> >();

    beginInsertRows(QModelIndex(), m_walletNodes.size(), m_walletNodes.size());
    QJsonObject newWalletNode;
    newWalletNode.insert("url", walletNodeData[0]);
    newWalletNode.insert("fee", walletNodeData[1]);
    newWalletNode.insert("latency", walletNodeData[2]);
    newWalletNode.insert("message", walletNodeData[3]);
    m_walletNodes.append(newWalletNode);
    endInsertRows();
    saveWalletNodes();
  }

  void WalletNodesModel::removeWalletNode(qint32 _row) {
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
  }

  void WalletNodesModel::walletInitCompleted(int _error, const QString& _error_text) {
    if (!_error) {
      QFile walletNodesFile(Settings::instance().getWalletNodesFile());
      if (walletNodesFile.open(QIODevice::ReadOnly)) {
        QJsonObject obj = QJsonDocument::fromJson(walletNodesFile.readAll()).object();
        if (obj.contains("remoteNodes")) {
          m_walletNodes = obj.value("remoteNodes").toArray();
        }
        walletNodesFile.close();
        if (!m_walletNodes.isEmpty()) {
          beginInsertRows(QModelIndex(), 0, m_walletNodes.size() - 1);
          endInsertRows();
        }
      }
    }
  }

  const QModelIndex WalletNodesModel::indexFromUrl(const QString& searchstring, const int& column) {
    QModelIndex index = match(WalletNodesModel::index(0,column,QModelIndex()),
    Qt::DisplayRole, searchstring, 1,
    Qt::MatchFlags(Qt::MatchExactly|Qt::MatchRecursive))
    .value(0);
    return index;
  }

  bool WalletNodesModel::setData(const QModelIndex& _index, const QVariant& _value, int _role) {
    if(_index.isValid() && _role == Qt::EditRole) {
      if (_index.column() < 0 || _index.column() > 2) return false;
      addWalletNode(_index, _value);
      removeWalletNode(_index.row());
      return true;
    } else {
      return false;
    }
  }
}
