#ifndef TABCOMMON_H
#define TABCOMMON_H

#include <QWidget>
#include <QJsonDocument>
#include <QVariantMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class TabCommon;
}
QT_END_NAMESPACE

// Api类接口测试
class TabCommon : public QWidget
{
    Q_OBJECT

public:
    explicit TabCommon(QWidget *parent = nullptr);
    ~TabCommon();

private slots:
    void on_pushButton_send_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_checkBox_stateChanged(int arg1);

    void update(QVariantMap ret);

private:
    Ui::TabCommon *ui;
};

#endif // TABCOMMON_H
