/****************************************************************
 *  RetroShare is distributed under the following license:
 *
 *  Copyright (C) 2006 - 2009 RetroShare Team
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 ****************************************************************/

#include <QDate>
#include <QMessageBox>
#include <QClipboard>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>

#include "rshare.h"
#include "ProfilePage.h"
#include "util/misc.h"
#include "util/DateTime.h"
#include <gui/RetroShareLink.h>
#include <gui/profile/ProfileManager.h>
#include <gui/statistics/StatisticsWindow.h>

#include <retroshare/rspeers.h> //for rsPeers variable
#include <retroshare/rsdisc.h> //for rsPeers variable

/** Constructor */
ProfilePage::ProfilePage(QWidget * parent, Qt::WindowFlags flags)
    : ConfigPage(parent, flags)
{
  /* Invoke the Qt Designer generated object setup routine */
  ui.setupUi(this);
  
  ui.headerFrame->setHeaderImage(QPixmap(":/icons/contacts_256.png"));
	ui.headerFrame->setHeaderText(tr("User Profile"));
	
	ui.avatarLabel->setOwnId();
  ui.avatarLabel->setFrameType(AvatarWidget::STATUS_FRAME);

  //connect(ui.copykeyButton, SIGNAL(clicked()), this, SLOT(copyPublicKey()));
  connect(ui.saveButton, SIGNAL(clicked()), this, SLOT(fileSaveAs()));
  connect(ui._includeSignatures_CB, SIGNAL(toggled(bool)), this, SLOT(load()));
  connect(ui._copyLink_PB, SIGNAL(clicked()), this, SLOT(copyRSLink()));
  //connect(ui.showStats_PB, SIGNAL(clicked()), this, SLOT(showStats()));

  // hide profile manager as it causes bugs when generating a new profile.
  //ui.profile_Button->hide() ;

  connect(ui.createNewNode_PB,SIGNAL(clicked()), this, SLOT(profilemanager()));

  ui.onlinesince->setText(DateTime::formatLongDateTime(Rshare::startupTime()));

	//ui.filterComboBox->setCurrentIndex(1);
	//ui.filterComboBox->setVisible(false);
}

void ProfilePage::profilemanager()
{
    ProfileManager().exec();
}
void ProfilePage::showEvent ( QShowEvent * /*event*/ )
{
    RsPeerDetails detail;
    if (rsPeers->getPeerDetails(rsPeers->getOwnId(),detail))
    {
        ui.name->setText(QString::fromUtf8(detail.name.c_str()));
        ui.country->setText(QString::fromUtf8(detail.location.c_str()));

        ui.peerid->setText(QString::fromStdString(detail.id.toStdString()));
        ui.pgpid->setText(QString::fromStdString(detail.gpg_id.toStdString()));
        ui.pgpfingerprint->setText(misc::fingerPrintStyleSplit(QString::fromStdString(detail.fpr.toStdString())));

        /* set retroshare version */
        ui.version->setText(Rshare::retroshareVersion(true));

        std::list<RsPgpId> ids;
        ids.clear();
        rsPeers->getGPGAcceptedList(ids);
        int friends = ids.size();

        ui.friendsEdit->setText(QString::number(friends));
        
        RetroShareLink link;
        link.createPerson(detail.gpg_id);

        ui.rskeylink->setText(link.toHtml());
        ui.rskeylink->setToolTip(link.title());
    }
	 load() ;
}


ProfilePage::~ProfilePage()
{
}

/** Saves the changes on this page */
bool
ProfilePage::save(QString &/*errmsg*/)
{
 	return true;
}

/** Loads the settings for this page */
void
ProfilePage::load()
{
    /* Loads ouer default Puplickey */
    ui.certplainTextEdit->setPlainText(QString::fromUtf8(rsPeers->GetRetroshareInvite(ui._includeSignatures_CB->isChecked()).c_str()));
}
void
ProfilePage::copyRSLink()
{
	RetroShareLink link ;
	RsPeerId ownId = rsPeers->getOwnId() ;

	if( link.createCertificate(ownId) )
	{
		QList<RetroShareLink> urls ;

		urls.push_back(link) ;
		RSLinkClipboard::copyLinks(urls) ;
		QMessageBox::information(this,
				"RetroShare",
				tr("A RetroShare link with your Public Key is copied to Clipboard, paste and send it to your"
					" friend via email or some other way"));
	}
	else
		QMessageBox::warning(this,
				tr("Error"),
				tr("Your certificate could not be parsed correctly. Please contact the developers."));
}
void
ProfilePage::copyPublicKey()
{
    QMessageBox::information(this,
                             tr("RetroShare"),
                             tr("Your Public Key is copied to Clipboard, paste and send it to your"
                                " friend via email or some other way"));
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui.certplainTextEdit->toPlainText());
}

bool ProfilePage::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly))
        return false;
    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("UTF-8"));
    ts << ui.certplainTextEdit->document()->toPlainText();
    return true;
}

bool ProfilePage::fileSaveAs()
{
    QString fn;
    if (misc::getSaveFileName(this, RshareSettings::LASTDIR_CERT, tr("Save as..."), tr("RetroShare Certificate (*.rsc );;All Files (*)"), fn)) {
        fileName = fn;
        return fileSave();
    }
    return false;
}

void ProfilePage::showStats()
{
    StatisticsWindow::showYourself();
}
