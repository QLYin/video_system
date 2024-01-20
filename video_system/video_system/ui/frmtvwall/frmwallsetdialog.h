#ifndef FRMWALLSETDIALOG_H
#define FRMWALLSETDIALOG_H
#include <QVariantMap>
#include <QDialog>

class QDialogButtonBox;
class QComboBox;
class frmWallSetDialog :  public QDialog
{
	Q_OBJECT

public:
	explicit frmWallSetDialog(QWidget* parent= nullptr);
	const QVariantMap userData();

private:
	QDialogButtonBox* buttonBox;
	QVariantMap m_userData;
	QComboBox * m_rowCombox;
	QComboBox * m_colCombox;
};

#endif // FRMWALLSETDIALOG_H