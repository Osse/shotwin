#include <gtest/gtest.h>

#include "filterflattenproxymodel.h"
#include "photoitem.h"
#include "shotwin.h"

#include <QAbstractItemModel>
#include <QCoreApplication>
#include <QSqlDatabase>

TEST(DbTest, OpenDataBase)
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:/shotwell/photo.db");
    EXPECT_TRUE(db.open());
}

class ShotwinTests : public ::testing::Test
{
public:
    Shotwin s;
    QModelIndex aprilIndex;
    QModelIndex index2017;
    void SetUp()
    {
        s.initModels();
        index2017 = s.getEventTree()->index(0, 0, QModelIndex());
        aprilIndex = s.getEventTree()->index(0, 0, index2017);
    }
};

TEST_F(ShotwinTests, EventTreeIsCorrect)
{
    auto eventTree = s.getEventTree();

    EXPECT_EQ(eventTree->rowCount(), 2) << "Two years in db";
    EXPECT_EQ(eventTree->rowCount(index2017), 2) << "Jan and April in 2017";
    EXPECT_EQ(eventTree->data(aprilIndex, Qt::DisplayRole).toString(), QString("april"));
    EXPECT_EQ(eventTree->rowCount(aprilIndex), 16) << "16 events in April 2014";
}

TEST_F(ShotwinTests, PhotoCount)
{
    auto photoListModel = dynamic_cast<FilterFlattenProxyModel<PhotoItem>*>(s.getPhotoList());

    photoListModel->setTopLevelIndex(QModelIndex());
    EXPECT_EQ(photoListModel->rowCount(QModelIndex()), 420);

    photoListModel->setTopLevelIndex(aprilIndex);
    EXPECT_EQ(photoListModel->rowCount(QModelIndex()), 138) << "Number of photos in April";
}

TEST_F(ShotwinTests, EventCounts)
{
    auto eventListModel = dynamic_cast<FilterFlattenProxyModel<EventItem>*>(s.getEventList());

    eventListModel->setTopLevelIndex(QModelIndex());
    EXPECT_EQ(eventListModel->rowCount(QModelIndex()), 23) << "All events";

    eventListModel->setTopLevelIndex(aprilIndex);
    EXPECT_EQ(eventListModel->rowCount(QModelIndex()), 16) << "16 events in April 2014";
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
