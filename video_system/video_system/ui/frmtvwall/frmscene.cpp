#include "frmscene.h"

#include <QDebug>
#include <QLabel>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>


frmScene::frmScene(QString name, int id, QWidget* parent)
{
	m_name = name;
	m_id = id;

	/*QWidget* top = new QWidget(this);
	top->setFixedHeight(8);
	m_closeBtn = new QPushButton(top);
	m_closeBtn->setFixedSize(8, 8);
	QIcon closeIcon(":/image/close-8.png");
	m_closeBtn->setIcon(closeIcon);
	connect(m_closeBtn, &QPushButton::clicked, this, [this]() {
		emit sceneClose(this);
		});
	auto topLayout = new QHBoxLayout(top);
	topLayout->setMargin(0);
	topLayout->setSpacing(0);
	topLayout->addStretch();
	topLayout->addWidget(m_closeBtn);*/

	m_imgBtn = new QPushButton(this);
	m_imgBtn->setObjectName("scene_img_btn");
	m_imgBtn->setFixedHeight(36);
	connect(m_imgBtn, &QPushButton::clicked, this,&frmScene::onSceneClicked);

	m_ckbox = new QCheckBox(this);
	m_ckbox->setText(m_name);
	m_ckbox->setFixedHeight(12);
	QString styleSheet = "QCheckBox {"
		"    font-size: 10px; "
		"    color: #ffffff;"
		"}"
		"QCheckBox::indicator {"
		"    width: 8px;"
		"    height: 8px;"
		"}"
		"QCheckBox::checked {"
		"     color: #1296db;"
		"}";;
	m_ckbox->setStyleSheet(styleSheet);
	connect(m_ckbox, &QCheckBox::stateChanged, this, &frmScene::onSceneChecked);
	m_imgBtn->setIcon(m_ckbox->isChecked() ? QIcon(":/image/scene-36-checked.png") : QIcon(":/image/scene-36.png"));

	auto mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	//mainLayout->addWidget(top);
	mainLayout->addWidget(m_imgBtn);
	mainLayout->addSpacing(4);
	mainLayout->addWidget(m_ckbox);
	mainLayout->addStretch();
}

frmScene::~frmScene()
{

}

void frmScene::onSceneClicked()
{
	QDialog dialog(this);
	QVBoxLayout* layout = new QVBoxLayout(&dialog);
	dialog.setLayout(layout);
	layout->setMargin(10);
	layout->setSpacing(20);

	QLabel* label = new QLabel(QString::fromLocal8Bit("请选择操作:"), &dialog);
	layout->addWidget(label);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	QPushButton* callButton = new QPushButton(QString::fromLocal8Bit("调入视频"), &dialog);
	QPushButton* cancelButton = new QPushButton(QString::fromLocal8Bit("删除场景"), &dialog);
	callButton->setFixedSize(100, 80);
	cancelButton->setFixedSize(100, 80);

	connect(callButton, &QPushButton::clicked, &dialog, &QDialog::accept);
	connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
	buttonLayout->addWidget(callButton);
	buttonLayout->addWidget(cancelButton);
	layout->addLayout(buttonLayout);

	if (dialog.exec() == QDialog::Accepted)
	{
		emit sceneCall(m_id);
	}
	else
	{
		emit sceneDelete(this);
	}
}

void frmScene::onSceneChecked(int state)
{
	m_imgBtn->setIcon(m_ckbox->isChecked() ? QIcon(":/image/scene-36-checked.png") : QIcon(":/image/scene-36.png"));
	emit checkStateChange(m_ckbox->isChecked());
}

QString frmScene::sceneName()
{
	return m_name;
}

int frmScene::sceneId()
{
	return m_id;
}

bool frmScene::isChecked()
{
	return m_ckbox->isChecked();
}