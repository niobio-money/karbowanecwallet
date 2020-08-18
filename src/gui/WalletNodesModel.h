// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QAbstractTableModel>
#include <QJsonArray>

namespace WalletGui {

class WalletNodesModel : public QAbstractTableModel
{
  Q_OBJECT
  Q_DISABLE_COPY(WalletNodesModel)

public:
  enum Columns { COLUMN_URL = 0, COLUMN_FEE, COLUMN_LATENCY, COLUMN_MESSAGE};
  enum Roles { ROLE_URL = Qt::UserRole, ROLE_FEE, ROLE_LATENCY, ROLE_MESSAGE };

  static WalletNodesModel& instance();
  int columnCount(const QModelIndex& _parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QVariant data(const QModelIndex& _index, int _role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  bool setData(const QModelIndex& _index, const QVariant& _value, int _role = Qt::EditRole) Q_DECL_OVERRIDE;
  Qt::ItemFlags flags(const QModelIndex& _index) const Q_DECL_OVERRIDE;
  QVariant headerData(int _section, Qt::Orientation _orientation, int _role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  int rowCount(const QModelIndex& _parent = QModelIndex()) const Q_DECL_OVERRIDE;
  void addWalletNode(const QModelIndex& _index, const QVariant& _value);
  void removeWalletNode(qint32 _row);
  const QModelIndex indexFromUrl(const QString& searchstring, const int& column);

private:
  QJsonArray m_walletNodes;

  WalletNodesModel();
  ~WalletNodesModel();

  void reset();
  void saveWalletNodes();
  void walletInitCompleted(int _error, const QString& _error_text);

};

}
