#pragma once
#include <QObject>
#include <QWidget>
#include <QDateTime>
#include <QLabel>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QTextEdit>
namespace QNplCad
{

	class FileChangedWarning : public QWidget
	{
		Q_OBJECT

	public:
		FileChangedWarning(QWidget *parent = nullptr)
			: QWidget(parent)
			, mLabel(new QLabel(this))
			, mButtons(new QDialogButtonBox(QDialogButtonBox::Yes |
				QDialogButtonBox::No,
				Qt::Horizontal,
				this))
		{
			mLabel->setText(tr("File change detected. Discard changes and reload the file?"));

			QHBoxLayout *layout = new QHBoxLayout;
			layout->addWidget(mLabel);
			layout->addStretch(1);
			layout->addWidget(mButtons);
			setLayout(layout);

			connect(mButtons, SIGNAL(accepted()), SIGNAL(reload()));
			connect(mButtons, SIGNAL(rejected()), SIGNAL(ignore()));
		}

	signals:
		void reload();
		void ignore();

	private:
		QLabel *mLabel;
		QDialogButtonBox *mButtons;
	};
	class Document : public QWidget {
		Q_OBJECT
	public:
		Document(QWidget* parent = 0);
		~Document();
		bool save(QString *error = nullptr);
		bool save(const QString &fileName, QString *error = nullptr);
		QString fileName() const { return mFileName; }
		QString displayName() const;
		bool isModified() const;
		QDateTime lastSaved() const { return mLastSaved; }
		bool loadFile(const QString &fileName, QString *error = nullptr);
		void setFileChangedWarningVisible(bool visible);
	signals:
		void fileNameChanged(const QString &fileName, const QString &oldFileName);
		void saved();
		void reload();
		void textChanged();
	private:
		void setFileName(const QString &fileName);
	private:
		QString mFileName;
		QDateTime mLastSaved;
		FileChangedWarning *mWarning;
		QTextEdit* mTextEdit;
	};
}
