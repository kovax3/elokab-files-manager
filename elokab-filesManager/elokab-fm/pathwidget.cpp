/***************************************************************************
 *   elokab Copyright (C) 2014 AbouZakaria <yahiaui@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "pathwidget.h"
#include "ui_pathwidget.h"
//#include <EIcon>
#include <EMimIcon>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QLineEdit>
#include <QLineEdit>
#include <QFontMetrics>
/*********************************************************************************
 *
 ********************************************************************************/
PathWidget::PathWidget(QWidget *parent) :
     QWidget(parent),
     ui(new Ui::PathWidget)
{
     ui->setupUi(this);
     QColor color=this->palette().dark().color();
     QColor color2=this->palette().light().color();
     QColor color3=this->palette().shadow().color();
     QString style=QString("QWidget #widgetPath {"
                           "\n      border:1px inset rgba(%7, %8, %9, 150);"
                           " border-radius: 5px;"
                           "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1,"
                           "stop:0 rgba(%4, %5, %6, 80),"
                           "stop:1 rgba(%1, %2, %3, 80));"
                           "}"
                           )
               .arg(color.red()).arg(color.green()).arg(color.blue())
               .arg(color2.red()).arg(color2.green()).arg(color2.blue())
               .arg(color3.red()).arg(color3.green()).arg(color3.blue());

     setStyleSheet(style);
     // setLayoutDirection(Qt::LeftToRight);
     ui->toolButtonEdit->setIcon(EIcon::fromTheme("document-edit","edit-rename"));
     connect( ui->toolButtonEdit,SIGNAL(toggled(bool)),this,SLOT(toolbarVisible(bool)));

     // ui->lineEdit->setText(url);
     ui->lineEdit->setVisible(false);
     mMenu=new QMenu;
     //    btnRoot->setMenu(mMenu);
//     mactRoot=new   QAction(tr("Root"),this);
//     connect(mactRoot,SIGNAL(triggered()),this,SLOT(goActionPath()));

     mToolBar=new QToolBar(this);
     ui->horizontalLayout->addWidget(mToolBar);
     mToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
     mToolBar->setIconSize(QSize(16,16));

     //wButtons=new WidgetButtons(ui->widget);
     actionsGroup = new QActionGroup(this);

     //wButtons->move(0,1);

     completer = new QCompleter(this);
     completer->setMaxVisibleItems(7);
     completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
     fsModel = new QFileSystemModel(completer);
     fsModel->setFilter(QDir::AllDirs|QDir::NoDotAndDotDot);
     fsModel->setRootPath("/");

     //wButtons->show();
     completer->setModel(fsModel);

     ui->lineEdit->setCompleter(completer);
     QSettings settings(QApplication::organizationName(),QApplication::applicationName());
     settings.beginGroup("Main");
     ui->toolButtonEdit->setChecked(settings.value("ShowEditLocation",false).toBool());
     settings.endGroup();

}
/*********************************************************************************
 *
 ********************************************************************************/
PathWidget::~PathWidget()
{
    qDebug()<<"PathWidget deleted";
     QSettings settings(QApplication::organizationName(),QApplication::applicationName());
     settings.beginGroup("Main");
     settings.setValue("ShowEditLocation",ui->toolButtonEdit->isChecked());
     //   ui->toolButtonEdit->setChecked(settings.value("ShowEditLocation",false).toBool());
     settings.endGroup();
     delete ui;
}

/*********************************************************************************
 *
 ********************************************************************************/
void  PathWidget::resizeEvent(QResizeEvent */*event*/)
{
     // ui->widget->setMinimumHeight(wButtons->height());
     // wButtons->resizeEvent(event);
    //qDebug()<<mToolBar->sizeHint().width()<<mToolBar->width();

}

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::setUrl(const QString &url)
{

    if(url==":/search")
        return;

   ui->lineEdit->setText(url); QString txt;

    if(mToolBar->actions().count()>0)
        txt=mToolBar->actions().last()->data().toString();

    if( txt.startsWith(url) ){

        foreach (QAction *act, mToolBar->actions()) {
            if(act->data().toString()==url){
                act->setChecked(true);
                 return;
            }
        }

    }



    setupToolUrl(url);
}

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::on_lineEdit_returnPressed()
{
     QDir dir(ui->lineEdit->text());
     if(!dir.exists()){

          qDebug()<<"PathWidget::on_lineEdit_returnPressed()===directory no exist :"+ui->lineEdit->text();
          return;
     }
     emit urlChanged(ui->lineEdit->text());
    // setupToolUrl(ui->lineEdit->text());
}

void PathWidget::toolbarVisible(bool checked)
{
     if(!checked){
         QDir dir(ui->lineEdit->text());
         if(!dir.exists()){
              qDebug()<<"PathWidget::on_lineEdit_returnPressed()===directory no exist :"+ui->lineEdit->text();
              urlChanged(QDir::homePath());

         }else
          setupToolUrl(ui->lineEdit->text());
     }
}

/*********************************************************************************
 *
 ********************************************************************************/
void PathWidget::setupToolUrl(const QString &url)
{
    if(ui->toolButtonEdit->isChecked())
        return;

    //     if(!QFile::exists(url))
    //          return;
    QString urlBtn=url;


    foreach (QAction *act, mToolBar->actions()) {
        act->blockSignals(true);
        // listActions.removeOne(act);
        delete act;
    }
    //  listActions.clear();
    //    foreach (QMenu *menu, listmenus) {
    //        menu->deleteLater();
    //    }

    //    listmenus.clear();
    mToolBar->clear();

    if(urlBtn==":/trash"){
        QAction *act=mToolBar->addAction
                (EIcon::fromTheme("user-trash","emptytrash"),tr("Trash"));
        QFont font;font.setBold(true);
        act->setFont(font);
        //   listActions.append(act);
        return;
    }
    QFontMetrics fm(mToolBar->font());



    QDir dir(urlBtn);
    if(!dir.exists()) return;

    QString text=dir.dirName();
    if(dir==QDir::rootPath())
        text=tr("Root");



    while (dir.cdUp()) {

        QString text=dir.dirName();
        if(dir==QDir::rootPath())
            text=tr("Root");

        if(text==":"||text.isEmpty())
            continue;

        QString  label=fm.elidedText(text,Qt::ElideRight,180);

        QAction *act=new QAction(EMimIcon::iconFolder(dir.path()),label,this);
        act->setToolTip(text);
        connect(act,SIGNAL(triggered()),this,SLOT(goActionPath()));

        act->setData(dir.absolutePath());
        act->setCheckable(true);
        //act->setMenu(menuDirs(dir));
        mToolBar->insertAction(mToolBar->actions().first(),act);
        //mToolBar->addAction(act);
        actionsGroup->addAction(act);

        //    listActions.append(act);

    }

    dir.setPath(urlBtn);
    QString  label=fm.elidedText(text,Qt::ElideRight,180);
    QAction *act=mToolBar->addAction(EMimIcon::iconFolder(urlBtn),label);
    act->setToolTip(text);
    // QFont font;font.setBold(true);
    act->setData(urlBtn);
    // act->setFont(font);
    act->setCheckable(true);
    act->setChecked(true);
    actionsGroup->addAction(act);

    connect(act,SIGNAL(triggered()),this,SLOT(goActionPath()));

    //  listActions.append(act);







    adjustActions();

}

/**************************************************************************************
 *
 **************************************************************************************/
//QMenu *PathWidget::menuDirs( QDir dir)
//{
//     QMenu *menu=new QMenu;
//     //QList<QAction *> list;
//     foreach (QString subDir, dir.entryList(QDir::AllDirs|  QDir::NoDotAndDotDot))
//     {
//          QFileInfo fi(dir.absolutePath()+"/"+subDir);

//          QAction *act=new QAction(fi.fileName(),this);
//          connect(act,SIGNAL(triggered()),this,SLOT(goActionPath()));

//          act->setData(fi.filePath());
//          menu->addAction(act);

//          listActions.append(act);
//     }
//     listmenus.append(menu);
//     //return list;

//     return menu;
//}

/**************************************************************************************
 *
 **************************************************************************************/
void PathWidget::goActionPath()
{
     QAction *act=qobject_cast<QAction *>(sender());
     if(act){
          QString path=act->data().toString();
          if(path.isEmpty()||path.isNull())
               path=QDir::rootPath();
          emit urlChanged(path);
    //    setUrl(path);
     }

}
void PathWidget::adjustActions()
{
//qDebug()<<"[1]"<<mToolBar->actions().count()<<actionsGroup->actions().count();
  if(mToolBar->actions().count()<2)return;

   int count=mToolBar->actions().count();

    for (int i = 0; i <count ; ++i) {

        if(mToolBar->actions().count()<2)
            return;

        mToolBar->adjustSize();

        if(mToolBar->sizeHint().width()>mToolBar->width()){
          QAction *act=  mToolBar->actions().first();
           //qDebug()<<act->text();
           mToolBar->removeAction(act);

          if(actionsGroup->actions().contains(act))
             actionsGroup->removeAction(act);

          delete act;
        }

    }
//qDebug()<<"[2]"<<mToolBar->actions().count()<<actionsGroup->actions().count();
}
