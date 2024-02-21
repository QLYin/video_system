#include "frmwallsetdialog.h"
#include <QComboBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

frmWallSetDialog::frmWallSetDialog(QWidget* parent)
{
	QStringList values;
	for (int i = 1; i < 20; ++i) {
		values << QString::number(i);
	}
	m_rowCombox= new QComboBox(this);
	m_rowCombox->addItems(values);
	m_rowCombox->setCurrentIndex(1);
	m_colCombox = new QComboBox(this);
	m_colCombox->addItems(values);
	m_colCombox->setCurrentIndex(1);

	QFormLayout * flayout = new QFormLayout;
	
	flayout->addRow(QString::fromLocal8Bit("行数"), m_rowCombox);
	flayout->addRow(QString::fromLocal8Bit("列数"),m_colCombox);
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|
		QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected,this, &QDialog::reject);
	QVBoxLayout* mainLayout =  new QVBoxLayout;
    mainLayout->addLayout(flayout);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);
	setWindowTitle(tr("Wall Set"));
}

const QVariantMap frmWallSetDialog::userData()
{
	m_userData.insert("rows", m_rowCombox->currentText().toInt());
	m_userData.insert("cols", m_colCombox->currentText().toInt());
	return m_userData;
}