// Copyright (c) 2019 Niobio Cash developers <helder.garcia@gmail.com>
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QWidget>
#include <QFrame>
#include <QMenu>
#include <QStyledItemDelegate>
Q_DECLARE_METATYPE(QVector<QString>)

namespace Ui {
  class WalletNodesFrame;
}

namespace WalletGui {

  class AddressProvider;

  class WalletNodesFrame : public QFrame {
    Q_OBJECT
    Q_DISABLE_COPY(WalletNodesFrame)

  public:
    WalletNodesFrame(QWidget* _parent);
    ~WalletNodesFrame();
    QModelIndex index;

    public Q_SLOTS:
    void setClicked();
  private:
    QScopedPointer<Ui::WalletNodesFrame> m_ui;
    AddressProvider* m_addressProvider;
    void onAddressFound(const QJsonObject& _remoteNodeData);
    void walletInitCompleted(int _error, const QString& _error_text);
  };
}
