// -*- Mode: c++ -*-
#ifndef GUIDEGRID_H_
#define GUIDEGRID_H_

// c++
#include <vector>

// qt
#include <QString>
#include <QDateTime>
#include <QEvent>

// myth
#include "mythscreentype.h"
#include "programinfo.h"
#include "programlist.h"
#include "channelutil.h"

using namespace std;

class ProgramInfo;
class TimeInfo;
class TV;
class QTimer;
class MythUIButtonList;
class MythUIGuideGrid;

#define MAX_DISPLAY_CHANS 12
#define MAX_DISPLAY_TIMES 36

typedef vector<PixmapChannel>   pix_chan_list_t;
typedef vector<pix_chan_list_t> pix_chan_list_list_t;

class JumpToChannel;
class MPUBLIC JumpToChannelListener
{
  public:
    virtual void GoTo(int start, int cur_row) = 0;
    virtual void SetJumpToChannel(JumpToChannel *ptr) = 0;
    virtual int  FindChannel(uint chanid, const QString &channum,
                             bool exact = true) const = 0;
};

class MPUBLIC JumpToChannel : public QObject
{
    Q_OBJECT

  public:
    JumpToChannel(
        JumpToChannelListener *parent, const QString &start_entry,
        int start_chan_idx, int cur_chan_idx, uint rows_disp);

    bool ProcessEntry(const QStringList &actions, const QKeyEvent *e);

    QString GetEntry(void) const { return entry; }

  public slots:
    virtual void deleteLater(void);

  private:
    ~JumpToChannel() {}
    bool Update(void);

  private:
    JumpToChannelListener *listener;
    QString  entry;
    int      previous_start_channel_index;
    int      previous_current_channel_index;
    uint     rows_displayed;
    QTimer  *timer;

    static const uint kJumpToChannelTimeout = 3500; // ms
};

class MPUBLIC GuideGrid : public MythScreenType, public JumpToChannelListener
{
    Q_OBJECT

  public:
    // Use this function to instantiate a guidegrid instance.
    static void RunProgramGuide(uint           startChanId,
                                const QString &startChanNum,
                                TV            *player = NULL,
                                bool           allowFinder = true);

    DBChanList GetSelection(void) const;

    virtual void GoTo(int start, int cur_row);
    virtual void SetJumpToChannel(JumpToChannel *ptr);

    bool Create(void);
    bool keyPressEvent(QKeyEvent *event);

    virtual void aboutToShow();
    virtual void aboutToHide();

  protected slots:
    void cursorLeft();
    void cursorRight();
    void cursorDown();
    void cursorUp();

    void scrollLeft();
    void scrollRight();
    void scrollDown();
    void scrollUp();

    void dayLeft();
    void dayRight();
    void pageLeft();
    void pageRight();
    void pageDown();
    void pageUp();
    void toggleGuideListing();
    void toggleChannelFavorite();
    void generateListings();

    void enter();
    void escape();

    void showProgFinder();
    void channelUpdate();
    void volumeUpdate(bool);
    void toggleMute();

    void quickRecord();
    void editRecording();
    void editScheduled();
    void customEdit();
    void deleteRule();
    void upcoming();
    void details();

    void customEvent(QEvent *event);

  protected:
      GuideGrid(MythScreenStack *parentStack,
              uint chanid = 0, QString channum = "",
              TV *player = NULL, bool allowFinder = true);
   ~GuideGrid();

  private slots:
    void updateTimeout(void);
    void refreshVideo(void);
    void updateInfo(void);
    void updateChannels(void);
    void updateJumpToChannel(void);

  private:
    void showMenu(void);

    int  FindChannel(uint chanid, const QString &channum,
                     bool exact = true) const;

    void fillChannelInfos(bool gotostartchannel = true);
    void fillTimeInfos(void);
    void fillProgramInfos(void);
    void fillProgramRowInfos(unsigned int row);

    void setStartChannel(int newStartChannel);

    PixmapChannel       *GetChannelInfo(uint chan_idx, int sel = -1);
    const PixmapChannel *GetChannelInfo(uint chan_idx, int sel = -1) const;
    uint                 GetChannelCount(void) const;
    int                  GetStartChannelOffset(int row = -1) const;

    ProgramList GetProgramList(uint chanid) const;
    uint GetAlternateChannelIndex(uint chan_idx, bool with_same_channum) const;

  private:
    bool  m_selectChangesChannel;
    int   m_selectRecThreshold;

    bool m_allowFinder;
    pix_chan_list_list_t m_channelInfos;
    QMap<uint,uint>      m_channelInfoIdx;

    TimeInfo    *m_timeInfos[MAX_DISPLAY_TIMES];
    ProgramList *m_programs[MAX_DISPLAY_CHANS];
    ProgramInfo *m_programInfos[MAX_DISPLAY_CHANS][MAX_DISPLAY_TIMES];
    ProgramList  m_recList;

    QDateTime m_originalStartTime;
    QDateTime m_currentStartTime;
    QDateTime m_currentEndTime;
    uint      m_currentStartChannel;
    uint      m_startChanID;
    QString   m_startChanNum;

    int m_currentRow;
    int m_currentCol;

    bool    m_showFavorites;
    bool    m_sortReverse;
    QString m_channelFormat;

    int  m_channelCount;
    int  m_timeCount;
    bool m_verticalLayout;

    QDateTime m_firstTime;
    QDateTime m_lastTime;

    TV     *m_player;
    bool    m_usingNullVideo;
    QTimer *previewVideoRefreshTimer;
    void    EmbedTVWindow(void);
    void    HideTVWindow(void);
    QRect   m_videoRect;

    QString m_channelOrdering;
    QString m_dateFormat;
    QString m_timeFormat;
    QString m_unknownTitle;
    QString m_unknownCategory;

    QTimer *m_updateTimer;

    QMutex            m_jumpToChannelLock;
    JumpToChannel    *m_jumpToChannel;
    bool              m_jumpToChannelEnabled;

    MythUIButtonList *m_timeList;
    MythUIButtonList *m_channelList;
    MythUIGuideGrid  *m_guideGrid;
    MythUIText       *m_dateText;
    MythUIText       *m_jumpToText;
    MythUIImage      *m_channelImage;
};

#endif
