///////////////////////////////////////////////////////////////////////////////
//  
// This software system consists of computer software and documentation.
// It contains trade secrets and confidential information which are proprietary
// to Everi Games, Inc.  Its use or disclosure in whole or in part without
// the express written permission of Everi Games, Inc. is prohibited.
//  
// This software system is also an unpublished work protected under the copyright
// laws of the United States of America.
//  
// Copyright (c) 2011-2024  Everi Games, Inc.  All Rights Reserved
//  
////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <regex>

#if WIN32
#include "directx/d3d9.h"
#include "Common/ShareCnt/ShareCnt.h"
#endif

#include "json/value.h"
#include "json/writer.h"

#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/Logger.h"
#include "Poco/NObserver.h"
#include "Poco/Path.h"
#include "Poco/Process.h"
#include "Poco/Thread.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Format.h"
#include "Poco/ScopedLock.h"
#if DEVELOPER
#include "Poco/Delegate.h"
#endif // DEVELOPER

#include "OgreApplication.h"
#include "OgreResources.h"
#include "OgreProfiler.h"
#include "Common/Business/Cabinet/Types.h"
#include "Common/Business/Cabinet/CabinetProxy.h"
#include "Common/Business/GamePlay/Notifications.h"
#include "Common/Business/ScreenLocks/ScreenLocks.h"
#include "Common/Foundation/Exceptions.h"
#include "Common/Foundation/OS/OS.h"
#include "Common/G2SBase/CabinetHelper.h"
#include "Common/G2SBase/LoadedPaytables.h"
#include "Common/Loggers/PerfLoadTimes.h"
#include "Common/GPMenu/GPMenu.h"

#include "CommonDefs/DLC.h"
#include "CommonDefs/GroupNames.h"
#include "CommonDefs/MsgBoxDisplayEvent.h"

#include "CustomizedOverlayManager.h"

#include "DataLayer/Attributes.h"
#include "DataLayer/PersistentStoreFactory.h"
#include "DataLayer/Repository.h"
#include "DataLayer/GameSpecificDataRecord.h"
#include "DataLayer/MultiGameManager.h"

#include "Common/DebugSupportLink/DebugSupportLink.h"

#include "Common/GPInclude/GPMessages.h"

#include "Common/Localization/Localization/Localization.h"
#include "Common/Localization/Localization/Localizer.h"
#include "Common/Localization/Localization/LocalizerBuilder.h"

#include "Common/Foundation/Notification/NotificationManager.h"

#include "Messaging/Message.h"
#include "Common/ModuleTask/ModuleNames.h"

#include "Common/Foundation/Path/PathInfo.h"

#include "v8/v8.h"

#include "CustomizedRoot.h"
#include "GameResourceScriptLoader.h"

#include "ScriptablePlugin.h"
#include "ScriptObjects/Features/GraphicUtil.h"
#include "ScriptObjects/ScriptAttractMessageCycler.h"
#include "ScriptObjects/ScriptAudio.h"
#include "ScriptObjects/ScriptBannersManager.h"
#include "ScriptObjects/ScriptFxManager.h"
#include "ScriptObjects/ScriptLocalizedText.h"
#include "ScriptObjects/ScriptLogger.h"
#include "ScriptObjects/ScriptGameOverController.h"
#include "ScriptObjects/ScriptMechReelsManager.h"
#include "ScriptObjects/ScriptMusicalBangup.h"
#include "ScriptObjects/ScriptOgreAnimation.h"
#include "ScriptObjects/ScriptOgreAnticipationController.h"
#include "ScriptObjects/ScriptOgreAutopickTimer.h"
#include "ScriptObjects/ScriptOgreBangupController.h"
#include "ScriptObjects/ScriptOgreBigWinController.h"
#include "ScriptObjects/ScriptOgreCamera.h"
#include "ScriptObjects/ScriptOgreColor.h"
#include "ScriptObjects/ScriptOgreGame.h"
#include "ScriptObjects/ScriptOgreGraphics.h"
#include "ScriptObjects/ScriptOgreLight.h"
#include "ScriptObjects/ScriptOgreLuckyDust.h"
#include "ScriptObjects/ScriptMathData.h"
#include "ScriptObjects/ScriptOverlayGroup.h"
#include "ScriptObjects/ScriptOgreMessageBus.h"
#include "ScriptObjects/ScriptOgreMillicent.h"
#include "ScriptObjects/ScriptOgreMouseFollower.h"
#include "ScriptObjects/ScriptOgrePressToChangeController.h"
#include "ScriptObjects/ScriptOgreRandom.h"
#include "ScriptObjects/ScriptOgreRenderTexture.h"
#include "ScriptObjects/ScriptOgreSceneManager.h"
#include "ScriptObjects/ScriptOgreSceneNode.h"
#include "ScriptObjects/ScriptOgreScreenshot.h"
#include "ScriptObjects/ScriptOgreScreenshotRenderer.h"
#include "ScriptObjects/ScriptOgreSlotGame.h"
#include "ScriptObjects/ScriptOgreBoltOnBonusGame.h"
#include "ScriptObjects/ScriptOgreComponentGame.h"
#include "ScriptObjects/ScriptOgreUserMessage.h"
#include "ScriptObjects/ScriptOgreVector2.h"
#include "ScriptObjects/ScriptOgreVector3.h"
#include "ScriptObjects/ScriptOgreVector4.h"
#include "ScriptObjects/ScriptOgreViewport.h"
#include "ScriptObjects/ScriptOgreWindow.h"
#include "ScriptObjects/ScriptPerformanceTimer.h"
#include "ScriptObjects/ScriptPlayerBalanceDisplay.h"
#include "ScriptObjects/ScriptReelsManager.h"
#include "ScriptObjects/ScriptRevolveTopper.h"
#include "ScriptObjects/Features/MathInfo.h"
#include "ScriptObjects/ScriptPats.h"
#include "ScriptObjects/ScriptTENowEntryKeypad.h"
#include "ScriptObjects/ScriptOgreLanguageManager.h"
#include "Common/CabinetUtils/RevolveMessaging.h"

#include "OgreApp/ScriptOgreSplineFollower.h"

#include "IdleScene.h"
#include "CautionScene.h"
#include "MasterUI/MasterUI.h"
#include "DemoMenuUI.h"
#include "ButtonModeActor.h"
#include "TextFieldActor.h"
#include "GameResourceArchiveFactory.h"
#include "PreloadResourceArchiveFactory.h"

#include "OgreOverlayElement.h"
#include "Common/Foundation/Timer/MicrosecondsSinceStart.h"

#include "ProfileGroups.h"

#include "spritelib/SpriteOverlayElementFactory.h"
#include "spritelib/ScriptOgreDesign.h"

#include "UiWidgets/include/TextRenderable.h"

#include "AtlasWidget/include/AtlasWidgetPlugin.h"
#include "BinkWidget/include/BinkWidgetPlugin.h"
#include "EGMObjects/src/EGMObjectsPlugin.h"

#include "ProgressiveDisplay/include/ProgressiveDisplayPlugin.h"
#include "BingoDisplay/include/BingoDisplayPlugin.h"

#include "OGREplugins/BrowserWidget/include/BrowserWidgetPlugin.h"

#include "OGREplugins/PackageManager/Package.h"
#include "PackageManager/PackageManagerPluginBase.h"
#include "DLCManager/include/DLCManagerPlugin.h"

#include "spritelib/ScriptOgreSprite.h"
#include "animscriptlib/ScriptOgreAnimScript.h"
#include "AnimatedTextureSource.h"

#include "eventmanagerlib/EventMgr.h"

#include "Common/Foundation/Timer/ElapsedTimer.h"
#include "Common/Foundation/StringHash.h"

#include "CustomizedSceneManager.h"
#include "spritelib/VertexDeclarationManager.h"
#include "Common/ProblemMitigation/ProblemMitigator.h"
#include "RenderQueueController.h"


// From #include "cef/include/cef_app.h"
// (The current include file requires additional CEF dependencies not available in this project)
extern void CefDoMessageLoopWork();

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

using namespace DataLayer;
using namespace Ogre;
using namespace MFORCE;
using namespace MFORCEScript;
using namespace MFORCE::Common;
using namespace MFORCE::Foundation;
using namespace MFORCE::G2S;
using namespace MFORCE::GamePlay;
using namespace MFORCE::GameEngine;
using namespace MFORCE::Messaging;

namespace OgreApplicationInternalNamespace
{
    const int ERRORCODE_GAMESCRIPT = 1;

    const Poco::UInt32 FRAME_HITCH_TRAP_THRESHOLD_INTERVAL = 50;

    const Ogre::PixelFormat RECALL_TEXTURE_FORMAT = Ogre::PF_B8G8R8;

    class FindByName
    {
    public:

        FindByName(const std::string& name) :
            mName(name)
        {}

        bool operator() (const MonitorData& md) const
        {
            return md.GetName() == mName;
        }

        bool operator() (const DataLayer::ThemeComponent& tc) const
        {
            return tc.Name == mName;
        }

    private:
        std::string mName;
    };

    OverlayMap assembleOverlayMap(const ResourceIdCollection& ids)
    {
        OverlayMap overlayMap;
        for (ResourceIdCollection::const_iterator it = ids.begin(); ids.end() != it; ++it)
        {
            Ogre::OverlayElement* element = GraphicUtil::getOverlayElement(*it);
            if (NULL != element)
                overlayMap[*it] = element;
        }
        return overlayMap;
    }
}
using namespace OgreApplicationInternalNamespace;

static ElapsedTimer totalLoadTime;
static ElapsedTimer scriptStartupTime;
//TODO - CB - Remove small chunk logging.
static ElapsedTimer smallChunkTime;

const std::string OgreApplication::MarqueeCoreAssetName = "MarqueeCoreAssets";

static void startSmallChunk()
{
    smallChunkTime.Reset();
    smallChunkTime.Start();
}

static void logSmallChunk(const std::string& label)
{
    if (smallChunkTime.IsRunning())
    {
        poco_debug(GetPerfLoadTimesLogger(), label + " : " + smallChunkTime.GetSecondsSinceStart());
    }

    startSmallChunk();
}

/////////////////////////////////////////////////////////////////////////////// 

template<> OgreApplication* Singleton<OgreApplication>::ms_Singleton = 0;
OgreApplication* OgreApplication::getSingletonPtr(void)
{
    return ms_Singleton;
}

OgreApplication& OgreApplication::getSingleton(void)
{
    assert(ms_Singleton);  return (*ms_Singleton);
}

OgreApplication::OgreApplication(IDisplayConfiguration& displayConfiguration, bool renderOnly) :
    OGRE_NAMED_MUTEX_CONSTRUCT(OGRE_RENDERING_RESOURCE_MUTEX_ID),
    mDisplayConfiguration(displayConfiguration),
    mRoot(NULL),
    mSceneMgr(NULL),
    mPrimaryWindow(NULL),
    mPrimaryViewport(NULL),
    mpListeners(),
    mpIdleScene(0),
    mpMasterUI(0),
    mBingoGame(NULL),
    mTargetFPS(60),
    mFrameHitchThreshold(0),
    mFrameHitchTrapThreshold(0),
    mHitchCount(0),
    mLinkDownCounts(MFORCE::Cabinet::LinkDownType::MAX, 0),
    mEvictCount(0),
    mReelIndex(-1),
    mCurrentReelLayout(),
    mMultiReelGaffingSupported(false),
#if WIN32
    mRenderLoopSharedCounter(NULL),
#endif
#if DEVELOPER
    mEnableReelTiming(false),
#endif
    mFrameRenderQueuePassCount(0),
    mFrameRenderQueueTime(0),
    mFrameRenderQueueCount(0),
    mFrameRenderObjectCount(0),
    mpScriptEngine(NULL),
    mpOgreLog(0),
    mpOgreLogManager(0),
    mLoadStatus(LOAD_IDLE),
    mFrameCount(0),
    mpUserMessageController(NULL),
    mpBingoModeController(NULL),
    mpHandpayController(NULL),
    mHibernating(false),
    mpBigWinController(NULL),
    mpHardware(NULL),
    mpAudio(NULL),
    mpCashoutController(NULL),
    mLastUpdateTime(0),
    mLastLowMemoryTime(0),
    mUpdateSpeedMode(UpdateNormal),
    mUpdateSpeed(1),
    mDoNextStep(false),
    mGameFeatures(NULL),
    mbRecordAllFrames(false),
    mbProfilerEnabled(false),
    mbToggleProfiler(false),
    mbGameInPlay(false),
    mCoinInMillicents(0),
    mCoinOutMillicents(0),
    mSeedNextResult(false),
    mAutoplayer(new Autoplayer(Poco::Logger::get(MODULENAME_GAMEENGINE))),
    mAutoSnapshot(new AutoSnapshot()),
    mTimebaseDelta(0.0f),
    mpSpriteFactory(NULL),
    mpAnimatedTextureSource(NULL),
    mpDesignResourceManager(NULL),
    mpSplineResourceManager(NULL),
    mHintsAllowed(false),
    mBonusGaffing(false),
    mRenderOnly(renderOnly),
    mButtonModeActor(NULL),
    mKeyboardShortcuts(this),
    mTrackRenderQueueStats(false),
    mCurrentFPS(0.0f),
    mSmoothedFPS(0.0f),
    mpFPSOverlay(NULL),
    mpFPSText(NULL),
    mpLineStreamFactory(NULL),
    mpFPSGraph(NULL),
    mbSmoothFrames(true),
    mbProfilingEnabled(false),
    mbFrameTime(false),
    mAltTabCount(0),
    mbShowOverdraw(false),
    mOverdrawPass(NULL),
    mDeltaCarryover(0),
    mFrameUpdateNotification(new FrameUpdateNotification()),
    mpCycleDelayGameFlowOverlay(),
    mpCycleDelayGameFlowText(),
    mActualFrameTime(0.0),
    mLastDeltasToAvgIdx(0),
    mLastDeltasToTrackIdx(0),
    mLastEvictTime(0),
    mTrackFrameRates(false),
    mReplayRecallModeActive(false),
    mReplayRestoreModeActive(false),
    mUnityReplayModeActive(false),
    mResetUnityTechUIURL(true),
#if DEVELOPER
    mFrameCounter(0),
#endif // DEVELOPER
    mEvaluationModeActive(false),
    mMGUIMaximized(false),
    mGameSpecificData(""),
    mRenderingAPIErrorCount(0),
    mLoadDependencyMgr(),
    mPendingNitroBonuses(),
    mpRenderQueueController(),
    mNitroBonusThemes(),
    mIsNitroBonusActive(false),
    mIsTENowActive(false),
    mShowSitAndGoLoadingScene(false),
    mActiveBundle(""),
    mbSwitchLanguage(false),
    mOgreAppThreadId(std::this_thread::get_id()),
    mIsInSitAndGoTransitionArc(false),
    mUnloadedPrimaryGameForSitAndGoTransition(false),
    mTechUIFPS(30),
    mDemoMenuBrowserOverrideName(""),
    mpDemoMenuUI(NULL),
    mResetWindowTimer(5000, 0)
{

    //setup default options
    mOptions["hostip"] = "1381@localhost";
    mOptions["ogrecfg"] = "ogre.cfg";
    mOptions["ogrelog"] = "log/ogre.log";
    mOptions["defaultgame"] = "";
    mOptions["pluginroot"] = "../ogreplugins/";
    mOptions["fps"] = "60";
    mOptions["techUIFps"] = "30";
    mOptions["hitchthreshold"] = "0";
    mOptions["debugwait"] = "0";
    mOptions["localuipath"] = "../../../web/local";
    mOptions["idlescreenbg"] = "IdleScreen/BgBlank";
    mOptions["suppressloading"] = "0";
    mOptions["proxyusername"] = "";
    mOptions["proxypassword"] = "";


#ifdef LINUX
    mOptions["pluginlist"] = "RenderSystem_GL";
    mOptions["renderer"] = "OpenGL Rendering Subsystem";
#else
    mOptions["pluginlist"] = "RenderSystem_Direct3D9";
    mOptions["renderer"] = "Direct3D9 Rendering Subsystem";
#endif

#ifdef DEVELOPER 
    if (MFORCE::G2S::Cabinet::ShowEmbeddedMechReelsEmulator())
    {
        mSeeThruMgr = new SeeThrough::SeeThroughGlassMgr();
    }
#endif
}

/// Standard destructor
OgreApplication::~OgreApplication()
{
    StopScriptEngine();
    for (WindowMap::iterator iter = mWindows.begin(); iter != mWindows.end(); ++iter)
    {
        WindowEventUtilities::removeWindowEventListener((*iter).second, this);
    }

    mRenderQueueInfoForFrame.clear();
    mFrameRenderQueueInfo.clear();

    NotificationManager::Instance().RemoveObserver(Poco::NObserver<OgreApplication, MFORCE::CreateTextFieldActorNotification>(*this, &OgreApplication::onCreateTextField));
    if (mpLineStreamFactory)
        OGRE_DELETE mpLineStreamFactory;
#ifdef DEVELOPER 
    if (mSeeThruMgr)
    {
       delete mSeeThruMgr;
    }

    for (auto rbf : mMemoryRenderBuffers)
    {
        delete rbf;
    }
#endif
}

void OgreApplication::initialize()
{
    mOgreAppThreadId = std::this_thread::get_id();

    //Init Game engine features
    UnifiedMessageBroadcast::instance();
    GameEngineMessageBroadcast::instance();
    MathInfo::instance();
    mKeyboardShortcuts.init();
    scriptStartupTime.Reset();

    std::string indicatorsToFilter = Poco::toUpper(Poco::Util::Application::instance().config().getString("display.StatusIndicatorsToFilterOut", ""));
    if (!indicatorsToFilter.empty())
    {
        std::regex regex(",");
        std::copy(std::sregex_token_iterator(indicatorsToFilter.begin(), indicatorsToFilter.end(), regex, -1), std::sregex_token_iterator(), std::back_inserter(mIndicatorsToFilterOut));
    }

    NotificationManager::Instance().AddObserver(Poco::NObserver<OgreApplication, MFORCE::CreateTextFieldActorNotification>(*this, &OgreApplication::onCreateTextField));

}

void OgreApplication::SetOption(const std::string& name, const std::string& val)
{
    mOptions[name] = val;
}

std::string OgreApplication::GetOption(const std::string& name, const std::string& defaultVal) const
{
    std::string optionVal = defaultVal;
    OptionsMap::const_iterator optionIt = mOptions.find(name);
    if (mOptions.end() != optionIt)
    {
        optionVal = optionIt->second;
    }
    return optionVal;
}

void OgreApplication::setGameInPlay(bool bFlag)
{
    poco_debug_f1(Poco::Logger::get(MODULENAME_GAMEENGINE), __FUNCTION__": Setting gameInPlay %b", bFlag);

    mbGameInPlay = bFlag;
}

/// Process a frame
bool OgreApplication::Process()
{
    if (scriptStartupTime.IsRunning())
    {
        poco_information(GetPerfLoadTimesLogger(), "-=" + mActiveGame.ThemeName + " Script Startup Time=- : " + scriptStartupTime.GetSecondsSinceStart());
        poco_information(GetPerfLoadTimesLogger(), "-=" + mActiveGame.ThemeName + " Total Load Time=- : " + totalLoadTime.GetSecondsSinceStart());
        
        logSmallChunk(mActiveGame.ThemeId + " Running ");

        Json::Value msg;
        msg["themeId"] = mActiveGame.ThemeId;
        msg["totalLoadTime"] = totalLoadTime.GetElapsedTimeSinceStartS();
        MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM,
            "GameLoadTimeNotification",
            MFORCE::Messaging::Message::EVENT,
            msg);
        SendEvent(msg);

        scriptStartupTime.Reset();
        totalLoadTime.Reset();
    }

#if OGRE_PROFILING == 1
    if (mbToggleProfiler)
    {
        Ogre::Profiler* pProf = Ogre::Profiler::getSingletonPtr();

        if (pProf && !mHibernating)
        {
            if (!mbProfilerEnabled)
            {
                pProf->reset();
                pProf->setEnabled(true);

                pProf->setUpdateDisplayFrequency(5);

                Ogre::Overlay* pOverlay = CustomizedOverlayManager::getSingleton().getByName("Profiler");

                if (pOverlay)
                    CustomizedOverlayManager::getSingletonPtr()->showOverlay(getPrimaryViewport(), pOverlay);

                mbProfilerEnabled = true;
            }
            else
            {
                Ogre::Overlay* pOverlay = CustomizedOverlayManager::getSingleton().getByName("Profiler");

                if (pOverlay)
                    CustomizedOverlayManager::getSingletonPtr()->hideOverlay(getPrimaryViewport(), pOverlay);

                pProf->setEnabled(false);

                mbProfilerEnabled = false;
            }
        }
        mbToggleProfiler = false;
    }
#endif

    // profile all Process scope
    OgreProfileGroup("Process", PROFILING_GROUP_UPDATE);

    // process any bundle preloading
    mPreloadManager.process();

    unsigned long long currentUpdateTime = MicrosecondsSinceStart();
#if DEVELOPER
    ProfilingData::ProfileMarker("Frame", "E", StringConverter::toString(mFrameCounter, 0), 0);
    mFrameCounter++;
#endif // DEVELOPER

    bool result = true;
    if (mLastUpdateTime == 0)
        mLastUpdateTime = currentUpdateTime;
    long long deltaMicroseconds = currentUpdateTime - mLastUpdateTime;

#if DEVELOPER
    if (mbProfilerEnabled)
    {
        Json::Value msg;
        MFORCEScript::ProfilingData::LastFrameProfileData(msg);
        if (mbRecordAllFrames || mRenderTime > 33000)
        {
            SendEvent(msg);
        }
    }



    ProfilingData::ProfileMarker("Frame", "B", StringConverter::toString(mFrameCounter, 0), 0);
#endif // DEVELOPER

    WindowEventUtilities::messagePump();
    for (auto listener : mpListeners)
    {
        listener->CaptureInputDevices();
    }

    mActualFrameTime = deltaMicroseconds * 0.000001f * mGameTimeScale.getTimeScale();
    mLastUpdateTime = currentUpdateTime;

    if (deltaMicroseconds != 0)
    {
        if (deltaMicroseconds >= 1000000)
        {
            // if fps < 1, display fps in decimal
            mCurrentFPS = 1.0f / (deltaMicroseconds * 0.000001f);
        }
        else
        {
            // display fps in next integer value
            mCurrentFPS = floor((1.0f / (deltaMicroseconds * 0.000001f)) + 0.5f);
        }
    }
    // try to detect video memory fragmentation and recover from it:
    videoMemFragCheck(deltaMicroseconds);

    // calculate avg fps, smooth frames if enabled
    if ((mUpdateSpeedMode != UpdatePaused) && mbSmoothFrames && (MonitorConfig::Instance().GetVSync()))
        deltaMicroseconds = smoothFrame(deltaMicroseconds);

    // speed up / slow down time
    switch (mUpdateSpeedMode)
    {
    case UpdatePaused:
        if (mDoNextStep)
        {
            deltaMicroseconds = 16667;
            mDoNextStep = false;
        }
        else
        {
            deltaMicroseconds = 0;
        }
        mActualFrameTime = deltaMicroseconds * 0.000001f * mGameTimeScale.getTimeScale();
        break;

    case UpdateSlow:
        deltaMicroseconds /= mUpdateSpeed;
        break;

    case UpdateFast:
        deltaMicroseconds *= mUpdateSpeed;
        break;
    }

    mTimebaseDelta = deltaMicroseconds * 0.000001f * mGameTimeScale.getTimeScale();

    if (mRoot)
    {
        OgreProfileGroup("Process (lock/render/events)", PROFILING_GROUP_UPDATE);

        // capture render queue stats for previous frame
        mFrameRenderQueueInfo = mRenderQueueInfoForFrame;

        mFrameRenderQueuePassCount = mRenderQueuePassCount;
        mFrameRenderObjectCount = mRenderObjectCount;
        mFrameRenderQueueCount = mRenderQueueCount;
        mFrameRenderQueueTime = mRenderQueueTime;
        if (mpFPSGraph)
        {
            mpFPSGraph->setTraceValue(4, mRenderTime / 1000.0f);
        }
        // clear render queue stats for current frame
        mRenderQueueInfoForFrame.clear();

        mRenderQueuePassCount = 0;
        mRenderObjectCount = 0;
        mRenderQueueCount = 0;
        mRenderQueueTime = 0;

#ifdef WIN32
        if (mRenderLoopSharedCounter != NULL)
        {
            // Increment the profiling counter
            IncrementSharedCounter(mRenderLoopSharedCounter);
        }
#endif
        if (mTrackFrameRates && mTargetFPS == mDefaultFPS && mCurrentFPS < getFrameHitchThreshold())
        {
            ++mHitchCount;
            if (mActualFrameTime > 0.0)
            {
                Ogre::String hitchmsg = Poco::format("*** FRAME HITCH DETECTED *** frames/sec: %hf, frame time (secs): %hf",
                    1.0f / mActualFrameTime, mActualFrameTime);
                LogManager::getSingleton().logMessage(hitchmsg, Ogre::LML_CRITICAL);
                evictManagedResources();
            }
            logGraphicsDeviceResourceInfo();
        }


        if (MFORCE::ProblemMitigation::ProblemMitigator::GetInstance()->GetAndResetAltTabSimulation())
        {
            SimulateAltTab();
        }

        if (CustomizedOverlayManager::getSingletonPtr()->wasResorted())
        {
            mRoot->setRemoveRenderQueueStructuresOnClear(true);
            CustomizedOverlayManager::getSingletonPtr()->clearResorted();
        }
        if (mpScriptEngine)
            mpScriptEngine->Update(deltaMicroseconds, mActualFrameTime);

        // scope for v8::Locker
        {
            // This v8::Locker may seem out of place here, but its purpose is to prevent other threads (prizegenerator) from 
            // doing some allocation/operation that triggers a v8 scavenge collection, which could result in deletion of a
            // a game engine native script object (that is a framelistener) in another thread,
            // while we're iterating through the frame listener std::set in the game engine thread.  
            // With the dangling pointer, it'd crash or freeze.
            //
            // v8 does garbage collection operations when it sees fit, not just when we call idlenotification.
            //
            // In the future, we should look into keeping a per-thread list of native objects to be deleted, and ensure that they
            // are deleted in the thread that they were created in.
            v8::Locker locker;

            // Ogre internally profiles renderOneFrame as "Frame"


#if DEVELOPER
            ProfilingData::ProfileMarker("Render", "B", "", 0);
#endif // DEVELOPER
            unsigned long long renderStart = MicrosecondsSinceStart();
            try
            {
#if DEVELOPER
                if (mSeeThruMgr)
                {
                    static int cnt = 0;
                    if (cnt <= 50)
                    {
                         cnt++;
                    }
                    else
                    {
                        mSeeThruMgr->TransparencyTask();
                    }
                }
#endif
                //if hibernating, only process a frame, do not render it
                if (!mHibernating)
                {
                    result = mRoot->renderOneFrame(mTimebaseDelta);
                    mRenderingAPIErrorCount = 0;
                }
                else
                {
                    result = mRoot->processOneFrame(mTimebaseDelta);
                }

            }
            catch (const Ogre::RenderingAPIException& e)
            {
                mRenderingAPIErrorCount++;

                poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Ogre::RenderingAPIExecption caught: " + e.getFullDescription());
                poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("RenderAPIErrorCount : %?u", mRenderingAPIErrorCount));
                Poco::Thread::sleep(100);
                if (mRenderingAPIErrorCount > MAX_RENDERING_API_ERROR_COUNT)
                    throw;
            }

            mRenderTime = MicrosecondsSinceStart() - renderStart;
#if DEVELOPER
            ProfilingData::ProfileMarker("Render", "E", "", 0);
#endif // DEVELOPER

            if (mpFPSOverlay)
                updateFPSDisplay();

            mRoot->setRemoveRenderQueueStructuresOnClear(false);
            mFrameCount++;

            EventMgr::instance().update();

        }
    }

    if (mpMasterUI && mpMasterUI->IsTextureStatsDisplayActive())
        mpMasterUI->UpdateTextureStatsDisplay();
    if (mpMasterUI && mpMasterUI->IsAnimationStatsDisplayActive())
        mpMasterUI->UpdateAnimationStatsDisplay();
#if DEVELOPER
    ProfilingData::ProfileMarker("Remainder", "B", "", 0);
#endif // DEVELOPER

    ProcessRemainingTime();
#if DEVELOPER
    ProfilingData::ProfileMarker("Remainder", "E", "", 0);
#endif // DEVELOPER

    NotificationManager::Instance().PostNotification(mFrameUpdateNotification);

    return result;
}

void OgreApplication::ProcessRemainingTime()
{
    OgreProfileGroup("ProcessRemainingTime", PROFILING_GROUP_UPDATE);

    std::string loadMsg = GetNextLoadMessage();
    if (loadMsg.empty() == false)
    {
        setLoadMessage(loadMsg);
        SetNextLoadMessage("");
    }
    switch (mLoadStatus)
    {
    case LOAD_TEXTURE:
        if ((mFrameCount % 5) == 0)
            updateElapsedMessage();
        break;
    case LOAD_TEXTUREPREPARED:
        texturesPrepared();
        break;
    case LOAD_DISK:
    {
        if ((mFrameCount % 5) == 0)
            updateElapsedMessage();
        if (mLoadError.empty() == false)
        {
            stopGame();
            setLoadMessage("\n" + mLoadError);
        }
        // try to get the frame rate close to the requested target frame rate (via sleeping)
        Poco::Thread::sleep(1000ULL / mTargetFPS);
    }
    break;
    case LOAD_UNLOADING:
    {
        AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
        if (pAtlasPlugin && !pAtlasPlugin->areGameTexturesLoaded())
        {
            mLoadStatus = LOAD_IDLE;
            onGameFinishedUnloading();
        }
    }
    break;
    default:
    {
        if (mTargetFPS != mDefaultFPS)
        {
            // we only really set a different fps when in the tech ui, 
            // sleep more so that the browser and server get more cpu.
            Poco::Thread::sleep(1000ULL / (mTargetFPS * 2));
        }
        else
        {
            Poco::Thread::sleep(1);
        }
        break;
    }
    }
}

void OgreApplication::SetCurrentReelIndex(const int reelIndex)
{
    mReelIndex = reelIndex;
}

void OgreApplication::SetMultiReelGaffingSupported(const bool supported)
{
    mMultiReelGaffingSupported = supported;
}

const bool OgreApplication::IsMultiReelGaffingSupported() const
{
    return mMultiReelGaffingSupported;
}

void OgreApplication::ActivateGaffingUI(const Json::Value& demoOptions, const Json::Value& demoSelections)
{
    if (mpMasterUI)
        mpMasterUI->ActivateGaffingUI(mCurrentReelLayout, demoOptions, demoSelections);
}

const bool OgreApplication::IsGaffingUIActive() const
{
    return mpMasterUI->IsGaffingUIActive();
}

void OgreApplication::ActivateBrowserPopup(const std::string& url)
{
    if (mpMasterUI)
        mpMasterUI->ActivateTechUIPopup(url);
}

void OgreApplication::setGameSpecificData(const std::string& gameSpecData)
{
    if (!mEvaluationModeActive)	//Don't persist data if in evaluation mode
    {
        DataLayer::GameSpecificDataRecord gameSpecDataRow(mActiveGame.ThemeId, mCurrentGameCycleID, gameSpecData);
        gameSpecDataRow.WriteGameRecord();
    }
    else						//Update Ogre variable if game persists data in eval mode
        mGameSpecificData = gameSpecData;

}

std::string OgreApplication::getGameSpecificData()
{
    if (!mEvaluationModeActive)
    {	//Find last record with the closest gameCycleID 
        DataLayer::GameSpecificDataRecord gameData(mActiveGame.ThemeId);
        return gameData.GetComponentWithCycleId(mCurrentGameCycleID).c_str();
    }
    else
        return mGameSpecificData;

}

void OgreApplication::FlushGPU(void)
{
#if WIN32
    mpOgreLog->logMessage("Flushing GPU", Ogre::LML_CRITICAL);
    // HACK-- force a res change to flush some direct3d managed memory :(
    DISPLAY_DEVICE dd = { 0 };
    dd.cb = sizeof(dd);
    DEVMODE devmode = { 0 };
    DWORD devNum = 0;
    while (EnumDisplayDevices(NULL, devNum, &dd, 0))
    {
        BOOL settings = EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &devmode);
        if (settings)
        {
            devmode.dmBitsPerPel >>= 1;
            ChangeDisplaySettingsEx(dd.DeviceName, &devmode, NULL, CDS_RESET | CDS_FULLSCREEN, NULL);
            devmode.dmBitsPerPel <<= 1;
            ChangeDisplaySettingsEx(dd.DeviceName, &devmode, NULL, CDS_RESET | CDS_FULLSCREEN, NULL);
        }
        ZeroMemory(&dd, sizeof(dd));
        dd.cb = sizeof(dd);
        devNum++;
    }
#endif
}

void OgreApplication::SimulateAltTab(void)
{
    const MonitorConfig& monitorConfig = MonitorConfig::Instance();

#if WIN32
    evictManagedResources();

    bool fullscreen = monitorConfig.GetFullscreen();
    if (fullscreen)
    {
        mAltTabCount++;
        mpOgreLog->logMessage(Poco::format("Attempting sim alt-tab... Count: %u", mAltTabCount), Ogre::LML_CRITICAL);
        HWND mainhwnd, hwnd;
        RenderWindow* pMainWindow = getPrimaryRenderWindow();
        pMainWindow->getCustomAttribute("WINDOW", &mainhwnd);

        for (WindowMap::iterator iter = mWindows.begin(); iter != mWindows.end(); ++iter)
        {
            (*iter).second->getCustomAttribute("WINDOW", &hwnd);
            if (hwnd != mainhwnd)
                ShowWindow(hwnd, SW_HIDE);
        }
        // hide main window last
        ShowWindow(mainhwnd, SW_HIDE);

        for (WindowMap::iterator iter = mWindows.begin(); iter != mWindows.end(); ++iter)
        {
            (*iter).second->getCustomAttribute("WINDOW", &hwnd);
            if (hwnd != mainhwnd)
                ShowWindow(hwnd, SW_SHOWNORMAL);
        }
        // Show main window last
        ShowWindow(mainhwnd, SW_SHOWNORMAL);

        // Activate, and focus main window
        PostMessage(mainhwnd, WM_ACTIVATEAPP, 1, 0);
        PostMessage(mainhwnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
        mpOgreLog->logMessage("Sim alt-tab complete", Ogre::LML_CRITICAL);
    }
#endif
}

void OgreApplication::LoadPlugins()
{

    std::string pluginroot = mOptions["pluginroot"];
    std::string pluginList = mOptions["pluginlist"];
    std::string optionalpluginList = mOptions["optionalpluginlist"];
    if (optionalpluginList.empty() == false)
    {
        pluginList += "," + optionalpluginList;
    }

    // DXVA2 depends on MediaFoundation DLLs, which started with Windows 7
    // so, filter it out if we're not running on 7 or later
    if (MFORCE::Common::OS::IsWindows7OrLater() == false)
    {
        std::string dxva2(",Plugin_DXVA2Video");
        std::string::size_type pos = pluginList.find(dxva2);
        if (pos != std::string::npos)
            pluginList.replace(pos, dxva2.length(), "");
    }

    Poco::StringTokenizer tokenizer(pluginList, ",", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
    for (int i = 0; i < tokenizer.count(); i++)
    {
        std::string tok = tokenizer[i];

        try
        {
            Poco::Path pluginPath(pluginroot);
            pluginPath.append(tok);
            mRoot->loadPlugin(pluginPath.toString());
        }
        catch (Ogre::InternalErrorException& e)
        {
            mpOgreLog->logMessage(e.getDescription(), Ogre::LML_CRITICAL);
        }
    }
    if (mRoot)
    {
        const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();
        std::for_each(installedPlugins.begin(), installedPlugins.end(), std::bind1st(std::mem_fun(&OgreApplication::InitializePluginScriptPrototypes), this));
    }
}

void OgreApplication::loadTransientPlugin(const char* pluginName)
{
    if (getPluginByName(pluginName) != nullptr)
    {
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::loadTransientPlugin() - plugin already loaded: %s", pluginName));
        return;
    }

    Poco::Path pluginPath(mOptions["pluginroot"]);
    pluginPath.append(pluginName);

    mRoot->loadPlugin(pluginPath.toString());

    Ogre::Plugin* pPlugin = getPluginByName(pluginName);
    if (pPlugin)
    {
        InitializePluginScriptPrototypes(pPlugin);
    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::loadTransientPlugin() - failed to load: %s", pluginName));
    }
}

void OgreApplication::unloadTransientPlugin(const char* pluginName)
{
    Ogre::Plugin* pPlugin = getPluginByName(pluginName);

    if (pPlugin == nullptr)
    {
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::unloadTransientPlugin() - plugin not loaded: %s", pluginName));
        return;
    }

    DeletePluginScriptObjectSingleton(pPlugin);

    Poco::Path pluginPath(mOptions["pluginroot"]);
    pluginPath.append(pluginName);

    mRoot->unloadPlugin(pluginPath.toString());

}

bool OgreApplication::hasComponentOwner(const std::string& componentName) const
{
    return mComponentOwners.find(componentName) != mComponentOwners.end();
}

bool OgreApplication::linkComponentToThemeId(const std::string& componentName, const std::string& themeId)
{
    if (themeId.empty())
    {
        return false;
    }

    auto& componentIter = mComponentOwners[componentName];
    if (componentIter.find(themeId) == componentIter.end())
    {
        componentIter.emplace(themeId);
        return true;
    }
    return false;
}

bool OgreApplication::linkComponentToThemeIdIfEmpty(const std::string& componentName, const std::string& themeId)
{
    if (!hasComponentOwner(componentName))
    {
        return linkComponentToThemeId(componentName, themeId);
    }
    return false;
}

void OgreApplication::delinkComponentThemeIds(const std::string& componentName)
{
    auto componentIter = mComponentOwners.find(componentName);
    if (componentIter != mComponentOwners.end())
    {
        mComponentOwners.erase(componentIter);
    }
}

void OgreApplication::delinkComponentsOfThemeId(const std::string& themeId)
{
    if (!themeId.empty())
    {
        for (auto iter = mComponentOwners.begin(); iter != mComponentOwners.end();)
        {
            auto componentIter = std::find(iter->second.begin(), iter->second.end(), themeId);
            if (componentIter != iter->second.end())
            {
                iter->second.erase(componentIter);
            }

            if (iter->second.empty())
            {
                iter = mComponentOwners.erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }
}

void OgreApplication::EnableWindowMultitouch(const std::string& name)
{
    const MonitorData& monitorData = MonitorConfig::Instance().GetMonitor(name);
    if (monitorData.IsValid() && monitorData.IsMultitouchEnabled())
    {
        poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::EnableWindowMultitouch - Enabling multitouch for %s", name));
        for (auto listener : mpListeners)
        {
            listener->DeviceWindowEnableMultitouch(name);
        }
    }
}

void OgreApplication::DisableWindowMultitouch(const std::string& name)
{
    const MonitorData& monitorData = MonitorConfig::Instance().GetMonitor(name);
    if (monitorData.IsValid() && monitorData.IsMultitouchEnabled())
    {
        poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::DisableWindowMultitouch - Disabling multitouch for %s", name));
        for (auto listener : mpListeners)
        {
            listener->DeviceWindowDisableMultitouch(name);
        }
    }
}

void OgreApplication::InitRenderer()
{
    std::string renderer = mOptions["renderer"];
    RenderSystem* pRsys = mRoot->getRenderSystemByName(renderer.c_str());
    if (!pRsys)
        OGRE_EXCEPT(Ogre::Exception::ERR_RENDERINGAPI_ERROR, "Invalid Rendering System", "OgreApplication::InitRenderer()");

    if (mOptions["renderer"].compare("Direct3D9 Rendering Subsystem") == 0)
    {
        pRsys->setConfigOption("Resource Creation Policy", "Create on active device");
        pRsys->setConfigOption("Floating-point mode", "Consistent");
        pRsys->setConfigOption("D3DCreateMode", "MultiThreaded");
    }

    pRsys->addListener(this);
    pRsys->_initRenderTargets();

    // Clear event times
    mRoot->clearEventTimes();
    mRoot->setRenderSystem(pRsys);
    mRoot->initialise(false);

    if (!mCustomSceneManagerFactory)
    {
        mCustomSceneManagerFactory = new CustomSceneManagerFactory();
        Ogre::Root::getSingletonPtr()->addSceneManagerFactory(mCustomSceneManagerFactory);
    }
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC, "CustomSceneManager");
}

void OgreApplication::InitLogging()
{
    //  Ogre singleton may have already been created
    mpOgreLogManager = Ogre::LogManager::getSingletonPtr();
    if (mpOgreLogManager == NULL)
    {
        mpOgreLogManager = OGRE_NEW Ogre::LogManager();
        mpOgreLog = mpOgreLogManager->createLog(mOptions["ogrelog"], true, false, true);
    }
    else
    {
        mpOgreLog = mpOgreLogManager->getLog(mOptions["ogrelog"]);
    }
    mpOgreLog->addListener(this);
    mpOgreLog->setLogDetail(Ogre::LL_NORMAL);
}

/** Sets up the application  */
bool OgreApplication::Startup()
{
    InitLogging();
    mpOgreLog->logMessage("OgreApplication Starting Up.");

    mFrameHitchThreshold = Poco::NumberParser::parseUnsigned(mOptions["hitchthreshold"]);
    if (mFrameHitchThreshold > 0)
    {
        mpOgreLog->logMessage(Poco::format("Frame hitch detection enabled with %?d FPS threshold", mFrameHitchThreshold));
    }
    mDefaultFPS = Poco::NumberParser::parseUnsigned(mOptions["fps"]);
    mTechUIFPS = Poco::NumberParser::parseUnsigned(mOptions["techUIFps"]);


    setTargetFPS(15);

    mRoot = OGRE_NEW CustomizedRoot("", mOptions["ogrecfg"], "");
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(new GameResourceArchiveFactory());
    Ogre::ArchiveManager::getSingleton().addArchiveFactory(new PreloadResourceArchiveFactory());
    mpSpriteFactory = new SpriteElementFactory(this);
    Ogre::OverlayManager::getSingleton().addOverlayElementFactory(mpSpriteFactory);

    mUpdaterManager.initialise();

    if (mpScriptEngine)
    {
        mScriptEngineUpdater.setScriptEngine(mpScriptEngine);
    }

    mpAnimatedTextureSource = new AnimatedTextureSource();
    ExternalTextureSourceManager::getSingleton().setExternalTextureSource(mpAnimatedTextureSource->getPlugInStringName(), mpAnimatedTextureSource);
    mpAnimatedTextureSource->initialise();

    LoadPlugins();
    InitRenderer();

    mpDesignResourceManager = new DesignResourceManager();
    mpSplineResourceManager = new SplineResourceManager();

    CreateWindows();

    bool gameWidgetsLoaded = false;

    loadCabinetMarqueeResource();

    if (mpMasterUI == NULL)
    {
        mpMasterUI = new MasterUI();
        gameWidgetsLoaded = mpMasterUI->Load();
        mpMasterUI->DisplayUnavailableScreens(true);
        mRoot->addFrameListener(mpMasterUI);
    }

    if (mpIdleScene == NULL)
    {
        mpIdleScene = new IdleScene(mOptions["idlescreenbg"]);
        mpIdleScene->Load();
        bool suppressLoading = (Poco::NumberParser::parseUnsigned(mOptions["suppressloading"]) != 0);
        mpIdleScene->ShowLoadingScene(!suppressLoading);
    }

    if (!mpCautionScene)
    {
        mpCautionScene = std::make_unique<CautionScene>();
        mpCautionScene->Load();
    }

    // if we're not loading a game (ex: simpleUI), no need for the demo menu
    if (gameWidgetsLoaded && mpDemoMenuUI == NULL)
    {
        mpDemoMenuUI = new DemoMenuUI();
    }

    LockSetForegroundWindow(LSFW_LOCK);

    createStockShaders();

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

    // listen for load events so we can display them.
    Ogre::ResourceGroupManager::getSingleton().addResourceGroupListener(this);
    mGameTimeScale.init();

#ifdef WIN32
    // Create a profiling counter for the render loop
    mRenderLoopSharedCounter = CreateSharedCounterWithProperties(L"OgreApplicationRenderLoopCounter", SC_PROP_COUNTER_DISABLED);
    if (mRenderLoopSharedCounter == NULL)
    {
        DWORD result = GetLastError();
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Failed to create shared counter 'OgreApplicationRenderLoopCounter', error = 0x%lx", result));
    }
#endif

    mpLineStreamFactory = OGRE_NEW Ogre::LineStreamOverlayElementFactory();
    OverlayManager::getSingleton().addOverlayElementFactory(mpLineStreamFactory);

    EventMgr::instance().init();

    return true;
}

void OgreApplication::StartScriptEngine(void)
{
    if (mRenderOnly == false)
    {
        mpScriptEngine = new ScriptEngine(new GameResourceScriptLoader());
        if (mpScriptEngine)
        {
            v8::Locker locker;
            const MonitorConfig& monitorConfig = MonitorConfig::Instance();

            std::string renderer = mOptions["renderer"];
            bool fullscreen = monitorConfig.GetFullscreen();
            bool vsync = monitorConfig.GetVSync();
            std::string gamename = mOptions["defaultgame"];

            //Setup some useful global vars for javascript
            mpScriptEngine->SetGlobal("renderer", renderer);
            mpScriptEngine->SetGlobal("fullscreen", fullscreen);
            mpScriptEngine->SetGlobal("vsync", vsync);
            mpScriptEngine->SetGlobal("defaultgame", gamename);

            ScriptObject<Animation>::instance()->Initialize("Animation", *mpScriptEngine);
            ScriptObject<AnimationScript>::instance()->Initialize("AnimationScript", *mpScriptEngine);
            ScriptObject<AnticipationController>::instance()->Initialize("AnticipationController", *mpScriptEngine);
            ScriptObject<Audio>::instance()->Initialize("audio", *mpScriptEngine);
            ScriptObject<AutopickTimer>::instance()->Initialize("AutopickTimer", *mpScriptEngine);
            ScriptObject<BangupController>::instance()->Initialize("Bangup", *mpScriptEngine);
            ScriptObject<BigWinController>::instance()->Initialize("bigWinController", *mpScriptEngine);
            ScriptObject<BingoModeController>::instance()->Initialize("BingoModeController", *mpScriptEngine);
            ScriptObject<Camera>::instance()->Initialize("Camera", *mpScriptEngine);
            ScriptObject<Color>::instance()->Initialize("Color", *mpScriptEngine);
            ScriptObject<Design>::instance()->Initialize("Design", *mpScriptEngine);
            ScriptObject<Events>::instance()->Initialize("events", *mpScriptEngine);
            ScriptObject<Game>::instance()->Initialize("game", *mpScriptEngine);
            ScriptObject<GameWindow>::instance()->Initialize("GameWindow", *mpScriptEngine);
            ScriptObject<ScriptGraphics>::instance()->Initialize("graphics", *mpScriptEngine);
            ScriptObject<HandpayController>::instance()->Initialize("handpayController", *mpScriptEngine);
            ScriptObject<Hardware>::instance()->Initialize("hardware", *mpScriptEngine);
            ScriptObject<CashoutController>::instance()->Initialize("cashoutController", *mpScriptEngine);
            ScriptObject<Light>::instance()->Initialize("Light", *mpScriptEngine);
            ScriptObject<LuckyDust>::instance()->Initialize("LuckyDust", *mpScriptEngine);
            ScriptObject<ScriptMathData>::instance()->Initialize("MathData", *mpScriptEngine);
            ScriptObject<ScriptMessageBus>::instance()->Initialize("MessageBus", *mpScriptEngine);
            ScriptObject<Millicent>::instance()->Initialize("Millicent", *mpScriptEngine);
            ScriptObject<MouseFollower>::instance()->Initialize("MouseFollower", *mpScriptEngine);
            ScriptObject<MusicalBangup>::instance()->Initialize("MusicalBangup", *mpScriptEngine);
            ScriptObject<ScriptMechReelsManager>::instance()->Initialize("MechReelsManager", *mpScriptEngine);
            ScriptObject<OverlayGroup>::instance()->Initialize("OverlayGroup", *mpScriptEngine);
            ScriptObject<PressToChangeController>::instance()->Initialize("PressToChangeController", *mpScriptEngine);
            ScriptObject<Random>::instance()->Initialize("random", *mpScriptEngine);
            ScriptObject<Revolve>::instance()->Initialize("Revolve", *mpScriptEngine);
            ScriptObject<SceneManager>::instance()->Initialize("scene", *mpScriptEngine);
            ScriptObject<ScriptAttractMessageCycler>::instance()->Initialize("AttractMessageCycler", *mpScriptEngine);
            ScriptObject<ScriptBannersManager>::instance()->Initialize("BannersManager", *mpScriptEngine);
            ScriptObject<ScriptFxManager>::instance()->Initialize("FxManager", *mpScriptEngine);
            ScriptObject<ScriptGameOverController>::instance()->Initialize("GameOverController", *mpScriptEngine);
            ScriptObject<ScriptLocalizedText>::instance()->Initialize("LocalizedText", *mpScriptEngine);
            ScriptObject<ScriptLogger>::instance()->Initialize("Logger", *mpScriptEngine);
            ScriptObject<ScriptTENowEntry>::instance()->Initialize("TENowEntry", *mpScriptEngine);
            ScriptObject<ScriptOgreSlotGame>::instance()->Initialize("SlotGame", *mpScriptEngine);
            ScriptObject<ScriptOgreSceneNode>::instance()->Initialize("SceneNode", *mpScriptEngine);
            ScriptObject<ScriptOgreScreenshot>::instance()->Initialize("ScreenshotController", *mpScriptEngine);
            ScriptObject<ScriptOgreScreenshotRenderer>::instance()->Initialize("ScreenshotRenderer", *mpScriptEngine);
            ScriptObject<ScriptPerformanceTimer>::instance()->Initialize("PerformanceTimer", *mpScriptEngine);
            ScriptObject<ScriptPlayerBalanceDisplay>::instance()->Initialize("PlayerBalanceDisplay", *mpScriptEngine);
            ScriptObject<ScriptReelsManager>::instance()->Initialize("ReelsManager", *mpScriptEngine);
            ScriptObject<ScriptRenderTexture>::instance()->Initialize("RenderTexture", *mpScriptEngine);
            ScriptObject<ScriptSplineFollower>::instance()->Initialize("SplineFollower", *mpScriptEngine);
            ScriptObject<UserMessageController>::instance()->Initialize("userMessage", *mpScriptEngine);
            ScriptObject<Vector2>::instance()->Initialize("Vector2", *mpScriptEngine);
            ScriptObject<Vector3>::instance()->Initialize("Vector3", *mpScriptEngine);
            ScriptObject<Vector4>::instance()->Initialize("Vector4", *mpScriptEngine);
            ScriptObject<Viewport>::instance()->Initialize("Viewport", *mpScriptEngine);
            ScriptObject<ScriptOgreBoltOnBonusGame>::instance()->Initialize("BoltOnBonusGame", *mpScriptEngine);
            ScriptObject<ScriptOgreComponentGame>::instance()->Initialize("ComponentGame", *mpScriptEngine);
            ScriptObject<Pats>::instance()->Initialize("Pats", *mpScriptEngine);
            ScriptObject<ScriptOgreLanguageManager>::instance()->Initialize("LanguageManager", *mpScriptEngine);

            WidgetScriptObject<Sprite, SpriteOverlayElement>::instance()->Initialize("Sprite", *mpScriptEngine);
        }
    }
}

void OgreApplication::StopScriptEngine(void)
{
    if (mpScriptEngine)
    {
        mScriptEngineUpdater.clearScriptEngine();
        mpScriptEngine->ShutDown();
        delete mpScriptEngine;
        mpScriptEngine = NULL;
    }
}

void OgreApplication::CreateWindows()
{
    const MonitorConfig& monitorConfig = MonitorConfig::Instance();

    bool fullscreen = monitorConfig.GetFullscreen();
    bool windowchrome = monitorConfig.GetWindowChrome();
    bool vsync = monitorConfig.GetVSync();

    // get actual number of monitors attached
    int maxMonitors = mRoot->getDisplayMonitorCount();
    // get number of monitors configured
    int numMonitors = monitorConfig.GetMonitorCount();

#if DEVELOPER
    bool bMechReelWindowEntryFound = false;
#endif
    // look for configured monitors
    for (int i = 0; i < numMonitors; i++)
    {
        const MonitorData& monitorData = monitorConfig.GetMonitor(static_cast<Poco::UInt8>(i));

#if DEVELOPER
        if (monitorData.GetName() == "MECH_REELS")
        {
            bMechReelWindowEntryFound = true;
        }
#endif

        if (monitorData.IsValid() && mWindows.find(monitorData.GetName()) == mWindows.end())
        {
            std::string monitorName = monitorData.GetName();

            Poco::UInt8 monitorOrdinal = monitorData.GetOrdinal();

            // check if we have enough monitors for this renderwindow (full screen only)
            if (fullscreen && (monitorOrdinal > (maxMonitors - 1)))
            {
                // log it out
                std::stringstream ssMsg;
                ssMsg << "Unable to create render window ";
                ssMsg << monitorName << ". ";
                ssMsg << "Configured monitors greater than detected monitors (" << maxMonitors << ").";
                mpOgreLog->logMessage(ssMsg.str(), LML_CRITICAL);

                break;
            }

            Poco::UInt8 monitorIndex = monitorData.GetIndex();
            MonitorData::MonitorRotation rotation = monitorData.GetRotation();

            // resolution
            int xres = monitorData.GetWidth();
            int yres = monitorData.GetHeight();
            if (rotation == MonitorData::kMonitorRotation90 || rotation == MonitorData::kMonitorRotation270)
                std::swap(xres, yres);

            NameValuePairList opts;
            opts["vsync"] = vsync ? "Yes" : "No";
            opts["left"] = Poco::NumberFormatter::format(monitorData.GetLeft());
            opts["top"] = Poco::NumberFormatter::format(monitorData.GetTop());

            // HACK.  only enable vsync for the first monitor when using opengl
            // otherwise, the frame rate is terrible.
            if (mOptions["renderer"].compare("OpenGL Rendering Subsystem") == 0)
            {
                if (monitorOrdinal != 0)
                {
                    opts["vsync"] = "No";
                }
            }

            opts["FSAA"] = "0";

            // omit "monitorIndex" in fullscreen so OGRE identifies monitor from (left, top) origin on virtual desktop
            if (!fullscreen)
            {
                // in windowed mode, "monitorIndex" identifies monitor on which window is created
                // and (left, top) is position of window with respect to the monitor 
                opts["monitorIndex"] = Poco::NumberFormatter::format(monitorIndex - 1);
            }

            // turn this on if you want to 
            // use NVidia Perf HUD
            //opts["useNVPerfHUD"] = "Yes";

            opts["border"] = windowchrome ? "fixed" : "none";

            char sbuf[1024];
            snprintf(sbuf, ARRAYSIZE(sbuf), "%s(%d,%d,%d,%d rot: %d index: %d) xres: %d yres: %d fullscreen: %s",
                monitorName.c_str(), monitorData.GetLeft(), monitorData.GetTop(), monitorData.GetWidth(), monitorData.GetHeight(),
                (int)monitorData.GetRotation(), (int)monitorData.GetIndex(), xres, yres, (fullscreen ? "true" : "false"));
            std::string monitorInfo = sbuf;

            try
            {
                poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::CreateWindows: Trying createRenderWindow(%s)", monitorInfo));

                RenderWindow* pWindow = mRoot->createRenderWindow(monitorName, xres, yres, fullscreen, &opts);

                // re-query the window metrics, way down in the ogre rendersystem
                pWindow->windowMovedOrResized();

                bool isTouchTypeDigitizer = (monitorData.GetTouchType() == MonitorData::MonitorTouchType::kMonitorTouchTypeDigitizer);

                unsigned int options = 0;
                if (isTouchTypeDigitizer)
                {
                    options = TouchTypeDigitizer;
                    if (monitorData.IsMultitouchEnabled()) options |= EnableMultiTouch;
                }

                windowOpened(pWindow, options);
                pWindow->addListener(this);

                if (isTouchTypeDigitizer)
                {
                    HWND windowHnd = 0;
                    pWindow->getCustomAttribute("WINDOW", &windowHnd);
                    mWindowsTouchManager.RegisterWindowForTouch(windowHnd);
                }
            }
            catch (Ogre::RenderingAPIException& e)
            {
                // This exception thrown here indicates an invalid ConfigMon setup, so lock the EGM using the DebugHost (in case the MAIN window is not present).
                mpOgreLog->logMessage(e.getFullDescription() + " (Unable to create render window - invalid monitor configuration)", Ogre::LogMessageLevel::LML_CRITICAL);

                DebugSupportLink& debugLink = DebugSupportLink::Instance();
                debugLink.LockScreen("Invalid Monitor Configuration", "Failed to create window " + monitorInfo + "\n\n" + e.getFullDescription());
            }
        }
    }

#if DEVELOPER
    if (!bMechReelWindowEntryFound && MFORCE::G2S::Cabinet::HasMechanicalReels())
    {
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Games with DMech style mechanical reels need MECH_REELS entry in hardware.xml");
    }
#endif

#if WIN32
    // set window focus to main screen
    if (!fullscreen)
    {
        RenderWindow* rw = getPrimaryRenderWindow();
        if (rw)
        {
            HWND windowHnd = nullptr;
            rw->getCustomAttribute("WINDOW", &windowHnd);
            SetForegroundWindow(windowHnd);
        }
    }
#endif
}

void OgreApplication::DestroyWindows()
{
    // Get the primary (main) window handle.
    HWND hwndMain = nullptr;
    RenderWindow* primaryRenderWindow = getPrimaryRenderWindow();
    primaryRenderWindow->getCustomAttribute("WINDOW", &hwndMain);

    // Loop through all the non-main active windows
    for (auto windowsIter = begin(mWindows); windowsIter != end(mWindows); )
    {
        // Get the HWND handle
        Ogre::RenderWindow* renderWindow = windowsIter->second;
        HWND hWnd = nullptr;
        renderWindow->getCustomAttribute("WINDOW", &hWnd);
        poco_assert(hWnd != nullptr);

        // Skip the the primary (main) window
        if (hWnd != hwndMain)
        {
            // attempting to destroy a child FullScreen window without minimizing first causes an unpredictable result 
            // (sometimes you can't focus any other windows on that monitor until the parent is focused first)
            // minimizing it first seems to address this.
            ShowWindow(hWnd, SW_MINIMIZE);
            windowClosed(renderWindow);
            Root::getSingleton().getRenderSystem()->destroyRenderWindow(windowsIter->first);
            windowsIter = mWindows.erase(windowsIter);
        }
        else 
        {
            ++windowsIter;
        }
    }

    ShowWindow(hwndMain, SW_MINIMIZE);
    windowClosed(primaryRenderWindow);
    Root::getSingleton().getRenderSystem()->destroyRenderWindow(primaryRenderWindow->getName());

    mWindows.clear();
    mPrimaryWindow = NULL;
    mPrimaryViewport = NULL;

}

void OgreApplication::ForceMaximizeWindows(Poco::Timer&)
{
    if (!MonitorConfig::Instance().GetFullscreen())
    {
        // Not fullscreen
        return;
    }

    WindowEventUtilities::messagePump();

    HWND primaryHWnd = 0;
    RenderWindow* primaryRenderWindow = getPrimaryRenderWindow();
    if (primaryRenderWindow != nullptr)
    {
        primaryRenderWindow->getCustomAttribute("WINDOW", &primaryHWnd);
    }

    for (auto windowsIter = begin(mWindows); windowsIter != end(mWindows); windowsIter++ )
    {
        HWND hWnd = 0;
        RenderWindow* renderWindow = windowsIter->second;

        renderWindow->getCustomAttribute("WINDOW", &hWnd);

        if (hWnd == primaryHWnd)
        {
            continue;
        }

        MaximizeWindow(hWnd);
    }

    MaximizeWindow(primaryHWnd);
    ActivateWindow(primaryHWnd);
}

void OgreApplication::MaximizeWindow(HWND hWnd)
{
    if (hWnd == 0)
    {
        return;
    }

    WINDOWPLACEMENT wndpl;
    GetWindowPlacement(hWnd, &wndpl);
    wndpl.showCmd = SW_SHOWMAXIMIZED;
    SetWindowPlacement(hWnd, &wndpl);
}

void OgreApplication::ActivateWindow(HWND hWnd)
{
    if (hWnd == 0)
    {
        return;
    }

    PostMessage(hWnd, WM_ACTIVATEAPP, 1, 0);
    PostMessage(hWnd, WM_ACTIVATE, WA_CLICKACTIVE, 0);
}

void OgreApplication::SetActiveTheme(const MFORCE::GamePlay::Game& gameToStart)
{
    mActiveGame = gameToStart;
}

void OgreApplication::Hibernate(void)
{
    if (mHibernating)
        return;

    mHibernating = true;

    mpScriptEngine->RemoveAllAddOnContext(bind(&OgreApplication::PurgeScriptObjects, this));

    hideFPSDisplay();

    if (mpMasterUI)
    {
        mRoot->removeFrameListener(mpMasterUI);
        mpMasterUI->Unload();
        delete mpMasterUI;
        mpMasterUI = NULL;
    }

    if (mpIdleScene)
    {
        mpIdleScene->Unload();
        delete mpIdleScene;
        mpIdleScene = NULL;
    }

    if (mpCautionScene)
    {
        mpCautionScene->Unload();
        mpCautionScene.reset();
    }

    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));
    if (pBrowserWidgetPlugin)
    {        
        mRoot->processOneFrame(0.01f);
        while (pBrowserWidgetPlugin->AreBrowsersBeingDestroyed())
        {
            mRoot->processOneFrame(0.01f);
        }            
    }

    //Force ALL overlay elements to destruct
    CustomizedOverlayManager::getSingleton().destroyAllOverlayElements(false, true);
    CustomizedOverlayManager::getSingleton().destroyAllOverlayElements(true, true);

    //Purge all master UI atlas textures and texture group
    auto* pAtlasPlugin = (AtlasWidgetPlugin*)getPluginByName("Plugin_AtlasWidget");
    if (pAtlasPlugin)
    {
        pAtlasPlugin->getTextureAtlas("MasterUI")->shutDown();
        pAtlasPlugin->OnMasterUIUnload();
    }

    DestroyWindows();
    releaseStockShaders();
    VertexDeclarationManager::getSingleton().clearAllDeclarations(false);
}

void OgreApplication::Awake(void)
{
    if (!mHibernating) 
    {
        return;
    }
    mHibernating = false;

    CreateWindows();

    if (mpMasterUI == NULL)
    {
        mpMasterUI = new MasterUI();
        mpMasterUI->Load();
        mpMasterUI->DisplayUnavailableScreens(true);
        mRoot->addFrameListener(mpMasterUI);
    }

    if (mpIdleScene == NULL)
    {
        mpIdleScene = new IdleScene(mOptions["idlescreenbg"]);
        mpIdleScene->Load();
        mpIdleScene->ShowLoadingScene(false);
    }

    if (!mpCautionScene)
    {
        mpCautionScene = std::make_unique<CautionScene>();
        mpCautionScene->Load();
    }

    if (mpDemoMenuUI == NULL)
    {
        mpDemoMenuUI = new DemoMenuUI();
    }

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);
    createStockShaders();
    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
    if (pAtlasPlugin)
    {
        pAtlasPlugin->getTextureAtlas("Game")->shutDown();
    }

    mResetWindowTimer.stop();
    Poco::TimerCallback<OgreApplication> forceMaximizeWindowCallback(*this, &OgreApplication::ForceMaximizeWindows);
    mResetWindowTimer.start(forceMaximizeWindowCallback);
}

void OgreApplication::LoadUnityAssets()
{
    // Load any Ogre resources needed for Unity Games (Marquee lighting in particular)
    DataLayer::ThemeComponentList componentList;
    OgreResources::Instance()->LoadResources(mActiveGame.ThemeId, componentList);
}

void OgreApplication::UnloadUnityAssets()
{
    //Force the Unity marquee lighting elements (and other Ogre resources used by Unity) to be destroyed
    OgreResources::Instance()->PurgeResources(mActiveGame.ThemeId);
    ResetTheme();
    if (mpDemoMenuUI)
    {
        delete mpDemoMenuUI;
        mpDemoMenuUI = NULL;
    }
}

void OgreApplication::LoadUnityBundleAssets()
{
    const DataLayer::MultiGameManager::BundleInfo* pBundleInfo = DataLayer::MultiGameManager::getInstance().getBundleInfo(mActiveBundle);
    mIsUnityBundle = DataLayer::MultiGameManager::getInstance().IsGameEngineTypeUnity(mActiveBundle);
    mKeyboardShortcuts.setIsUnityGameType(mIsUnityBundle);
    if (pBundleInfo)
    {
        // Load any Ogre resources needed for Unity bundle games (Marquee lighting in particular)
        DataLayer::ThemeComponentList componentList;
        for (const auto& theme : pBundleInfo->mThemes)
        {
            OgreResources::Instance()->LoadResources(theme.mThemeId, componentList);
        }

        Json::Value msg;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM, "SuggestLoadPackage", MFORCE::Messaging::Message::EVENT, msg);
        msg[Json::StaticString("pluginRequestType")] = "MultiGameUIPackageRequest";
        msg[Json::StaticString("packagePath")] = pBundleInfo->mUIPath;
        msg[Json::StaticString("context")] = Json::Value();
        SendEvent(msg);
    }
}

void OgreApplication::UnloadUnityBundleAssets()
{
    mIsUnityBundle = false;

    const DataLayer::MultiGameManager::BundleInfo* pBundleInfo = DataLayer::MultiGameManager::getInstance().getBundleInfo(mActiveBundle);
    if (pBundleInfo)
    {
        //Force the Unity marquee lighting elements (and other Ogre resources used by Unity) to be destroyed
        for (const auto& theme : pBundleInfo->mThemes)
        {
            OgreResources::Instance()->PurgeResources(theme.mThemeId);
        }

        Json::Value msg;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM, "SuggestUnloadPackage", MFORCE::Messaging::Message::EVENT, msg);
        msg[Json::StaticString("pluginRequestType")] = "MultiGameUIPackageRequest";
        msg[Json::StaticString("packagePath")] = pBundleInfo->mUIPath;
        SendEvent(msg);
    }
}

void OgreApplication::Shutdown(void)
{
#ifdef WIN32
    if (mRenderLoopSharedCounter != NULL)
    {
        DestroySharedCounter(mRenderLoopSharedCounter);
        mRenderLoopSharedCounter = NULL;
    }
#endif

    stopGame(false);

    EventMgr::instance().kill();	// kill sets the kill flag
    EventMgr::instance().update();	// update only processes the kill flag if it is set

    mWindowsTouchManager.Shutdown();
    Ogre::ResourceGroupManager::getSingleton().removeResourceGroupListener(this);

    mpMasterUI->EnableDemoMenuButton(false);

    if (mpDemoMenuUI)
    {
        delete mpDemoMenuUI;
        mpDemoMenuUI = NULL;
    }

    if (mpMasterUI)
    {
        mRoot->removeFrameListener(mpMasterUI);
        mpMasterUI->Unload();
        delete mpMasterUI;
        mpMasterUI = NULL;
    }
    
    unloadCabinetMarqueeResource();

    if (mpIdleScene)
    {
        mpIdleScene->Unload();
        delete mpIdleScene;
        mpIdleScene = NULL;
    }

    if (mpCautionScene)
    {
        mpCautionScene->Unload();
        mpCautionScene.reset();
    }

    if (mpDesignResourceManager)
    {
        delete mpDesignResourceManager;
        mpDesignResourceManager = NULL;
    }

    if (mpSplineResourceManager)
    {
        delete mpSplineResourceManager;
        mpSplineResourceManager = NULL;
    }

    if (mpScriptEngine)
    {
        v8::Locker locker;
        ScriptObject<Animation>::deleteInstance();
        ScriptObject<AnimationScript>::deleteInstance();
        ScriptObject<AnticipationController>::deleteInstance();
        ScriptObject<Audio>::deleteInstance();
        ScriptObject<AutopickTimer>::deleteInstance();
        ScriptObject<BangupController>::deleteInstance();
        ScriptObject<BigWinController>::deleteInstance();
        ScriptObject<BingoModeController>::deleteInstance();
        ScriptObject<Camera>::deleteInstance();
        ScriptObject<Color>::deleteInstance();
        ScriptObject<Design>::deleteInstance();
        ScriptObject<Events>::deleteInstance();
        ScriptObject<Game>::deleteInstance();
        ScriptObject<GameWindow>::deleteInstance();
        ScriptObject<ScriptGraphics>::deleteInstance();
        ScriptObject<HandpayController>::deleteInstance();
        ScriptObject<Hardware>::deleteInstance();
        ScriptObject<CashoutController>::deleteInstance();
        ScriptObject<Light>::deleteInstance();
        ScriptObject<LuckyDust>::deleteInstance();
        ScriptObject<ScriptMathData>::deleteInstance();
        ScriptObject<OverlayGroup>::deleteInstance();
        ScriptObject<ScriptMessageBus>::deleteInstance();
        ScriptObject<Millicent>::deleteInstance();
        ScriptObject<MouseFollower>::deleteInstance();
        ScriptObject<MusicalBangup>::deleteInstance();
        ScriptObject<PressToChangeController>::deleteInstance();
        ScriptObject<Random>::deleteInstance();
        ScriptObject<Revolve>::deleteInstance();
        ScriptObject<SceneManager>::deleteInstance();
        ScriptObject<ScriptAttractMessageCycler>::deleteInstance();
        ScriptObject<ScriptBannersManager>::deleteInstance();
        ScriptObject<ScriptFxManager>::deleteInstance();
        ScriptObject<ScriptGameOverController>::deleteInstance();
        ScriptObject<ScriptLocalizedText>::deleteInstance();
        ScriptObject<ScriptLogger>::deleteInstance();
        ScriptObject<ScriptMechReelsManager>::deleteInstance();
        ScriptObject<ScriptOgreSceneNode>::deleteInstance();
        ScriptObject<ScriptOgreScreenshot>::deleteInstance();
        ScriptObject<ScriptOgreScreenshotRenderer>::deleteInstance();
        ScriptObject<ScriptOgreSlotGame>::deleteInstance();
        ScriptObject<ScriptPerformanceTimer>::deleteInstance();
        ScriptObject<ScriptPlayerBalanceDisplay>::deleteInstance();
        ScriptObject<ScriptReelsManager>::deleteInstance();
        ScriptObject<ScriptRenderTexture>::deleteInstance();
        ScriptObject<ScriptSplineFollower>::deleteInstance();
        ScriptObject<UserMessageController>::deleteInstance();
        ScriptObject<Vector2>::deleteInstance();
        ScriptObject<Vector3>::deleteInstance();
        ScriptObject<Vector4>::deleteInstance();
        ScriptObject<Viewport>::deleteInstance();
        ScriptObject<ScriptOgreBoltOnBonusGame>::deleteInstance();
        ScriptObject<ScriptOgreComponentGame>::deleteInstance();
        ScriptObject<Pats>::deleteInstance();
        ScriptObject<ScriptOgreLanguageManager>::deleteInstance();

        WidgetScriptObject<Sprite, SpriteOverlayElement>::deleteInstance();

        const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
        std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::DeletePluginScriptObjectSingleton), this));
    }
    if (mpOgreLog)
    {
        mpOgreLog->logMessage("OgreApplication Shutting Down.");
        mpOgreLog->removeListener(this);
    }

    mGameTimeScale.shutdown();

    //Force ALL overlay elements to destruct
    CustomizedOverlayManager::getSingleton().destroyAllOverlayElements(false, true);
    CustomizedOverlayManager::getSingleton().destroyAllOverlayElements(true, true);

    std::string pluginroot = mOptions["pluginroot"];

    const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();

    // unload plugins in reverse order, in case of dependencies like tournevent->atlaswidget
    Ogre::Root::PluginInstanceList::const_reverse_iterator i = installedPlugins.rbegin();
    while (i != installedPlugins.rend())
    {
        ScriptablePlugin* pPlugin = dynamic_cast<ScriptablePlugin*>(*i);
        if (pPlugin)
        {
            Poco::Path pluginPath(pluginroot);
            pluginPath.append(pPlugin->getName());

            mRoot->unloadPlugin(pluginPath.toString());
            i = installedPlugins.rbegin();
        }
        else
        {
            ++i;
        }
    }

    delete mpSpriteFactory;
    mpSpriteFactory = NULL;
    delete mpAnimatedTextureSource;
    mpAnimatedTextureSource = NULL;

    releaseStockShaders();

    mUpdaterManager.shutdown();

    mSceneMgr->removeRenderQueueListener(this);
    mSceneMgr->removeRenderObjectListener(this);
    Ogre::MaterialManager::getSingleton().removeAll();
    VertexDeclarationManager::getSingleton().clearAllDeclarations(false);
    OGRE_DELETE mRoot;

    mRoot = NULL;

    // OgreRoot's destructor deletes the log manager,
    // and the log manager handles deleting the log instance
    mpOgreLog = NULL;
    mpOgreLogManager = NULL;

    // also destroyed by OgreRoot
    mSceneMgr = NULL;
}

void OgreApplication::systemReady()
{
    if (mActiveBundle.empty() && mActiveGame.ThemeId.empty())
    {
        mpIdleScene->ShowLoadingScene(false);
    }
    DataLayer::Attributes attributes;
    attributes.getConfigValue("gameengine.DemoHintsEnable", mHintsAllowed);
}

void OgreApplication::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
{
    Ogre::Viewport* pPort = evt.source;
    CustomizedOverlayManager::getSingleton().enableOverlaysForViewport(pPort);
}

void OgreApplication::viewportRemoved(const Ogre::RenderTargetViewportEvent& evt)
{
    Ogre::Viewport* pPort = evt.source;
    CustomizedOverlayManager* om = CustomizedOverlayManager::getSingletonPtr();
    if (om)
        om->viewportRemoved(pPort);

    CompositorDirector* cd = CompositorDirector::getSingletonPtr();
    if (cd)
        cd->viewportRemoved(pPort);
}

void OgreApplication::scriptParseStarted(const String& scriptName, bool& skipThisScript)
{
    // this callback is called from the bg thread, so we really shouldn't change the IdleScene caption, 
    // since the main thread could be in the process of rendering it.
    SetNextLoadMessage(std::string("Script " + scriptName));
}

void OgreApplication::resourcePrepareStarted(const ResourcePtr& resource)
{
    // this callback is called from the bg thread (see above note)
    SetNextLoadMessage(std::string("Resource " + resource->getName()));
}

void OgreApplication::onContextCreated(const std::string& ctxName, v8::Persistent<v8::Context>& ctx)
{
    v8::Locker locker;
    v8::HandleScope hs;
    v8::Context::Scope ctxTgt(ctx);

    mpAudio = ScriptObject<Audio>::instance()->CreateInstance("audio", ctx);
    ScriptObject<Random>::instance()->CreateInstance("random", ctx);
    ScriptObject<ScriptGraphics>::instance()->CreateInstance("graphics", ctx);
    ScriptObject<Events>::instance()->CreateInstance("events", ctx);
    ScriptObject<SceneManager>::instance()->CreateInstance("scene", ctx);
    ScriptObject<ScriptLocalizedText>::instance()->CreateInstance("localizedText", ctx);

    ScriptObject<Game>::instance()->CreateInstance("game", ctx);

    mpBigWinController = ScriptObject<BigWinController>::instance()->CreateInstance("bigWinController", ctx);
    mpBingoModeController = ScriptObject<BingoModeController>::instance()->CreateInstance("BingoModeController", ctx);
    mpHandpayController = ScriptObject<HandpayController>::instance()->CreateInstance("handpayController", ctx);
    mpHardware = ScriptObject<Hardware>::instance()->CreateInstance("hardware", ctx);
    mpCashoutController = ScriptObject<CashoutController>::instance()->CreateInstance("cashoutController", ctx);
    mpUserMessageController = ScriptObject<UserMessageController>::instance()->CreateInstance("userMessage", ctx);
    
    mpUserMessageController->setGameplayMessage(mLastGamePlayMessage);
    mpUserMessageController->setEGMMessage(mLastEGMMessage);

    if (MFORCE::Common::MonitorConfig::Instance().HasMechanicalWheels())
    {
        mpRevolve = ScriptObject<Revolve>::instance()->CreateInstance("revolve", ctx);
    }

#if DEVELOPER
    reelTimingEvent.userMessageController = mpUserMessageController;
    reelTimingEvent.ogreApplication = this;
    ScriptReelsManager::ReelTimingEvent.StartEvent += Poco::delegate(&reelTimingEvent, &ReelTimingEvent::Start);
    ScriptReelsManager::ReelTimingEvent.StopEvent += Poco::delegate(&reelTimingEvent, &ReelTimingEvent::Stop);
#endif // DEVELOPER

    // Store cashout UI pointer in cashout controller script object for customization access.
    mpCashoutController->setUIPointer(mpMasterUI->getCashoutMessageUI());

    if (mRoot)
    {
        const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();
        MFORCE::STLExtensions::for_each(MFORCE::STLExtensions::MakeIseq(installedPlugins), std::bind(&OgreApplication::PluginOnContextCreated, this, std::placeholders::_1, ctx));
    }

    for (auto listener : mpListeners)
    {
        listener->ScriptContextCreated(ctx);
    }
}

void OgreApplication::onComponentCreated(const std::string& ctxName, v8::Persistent<v8::Context>& ctx)
{
    v8::Locker locker;
    v8::HandleScope hs;
    v8::Context::Scope ctxTgt(ctx);

    ScriptObject<Audio>::instance()->CreateInstance("audio", ctx);
    ScriptObject<Random>::instance()->CreateInstance("random", ctx);
    ScriptObject<ScriptGraphics>::instance()->CreateInstance("graphics", ctx);
    ScriptObject<Events>::instance()->CreateInstance("events", ctx);
    ScriptObject<SceneManager>::instance()->CreateInstance("scene", ctx);
    ScriptObject<ScriptLocalizedText>::instance()->CreateInstance("localizedText", ctx);

    if (MFORCE::Common::MonitorConfig::Instance().HasMechanicalWheels())
    {
        ScriptObject<Revolve>::instance()->CreateInstance("revolve", ctx);
    }

    // TO-DO - CB - This should only be done for BOB components.
    ScriptObject<Game>::instance()->CreateInstance("game", ctx);
    ScriptObject<Hardware>::instance()->CreateInstance("hardware", ctx);

    if (mRoot)
    {
        const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();
        MFORCE::STLExtensions::for_each(MFORCE::STLExtensions::MakeIseq(installedPlugins), std::bind(&OgreApplication::PluginOnContextCreated, this, std::placeholders::_1, ctx));
    }

    for (auto listener : mpListeners)
    {
        listener->ScriptContextCreated(ctx);
    }
}

void OgreApplication::onContextTerminating(const std::string& ctxName, v8::Persistent<v8::Context>& ctx)
{
    v8::Locker locker;
    v8::HandleScope hs;
    v8::Context::Scope ctxTgt(ctx);

    ScriptObject<Audio>::instance()->DestroyInstance("audio", ctx);
    ScriptObject<Random>::instance()->DestroyInstance("random", ctx);
    ScriptObject<ScriptGraphics>::instance()->DestroyInstance("graphics", ctx);
    ScriptObject<Events>::instance()->DestroyInstance("events", ctx);
    ScriptObject<SceneManager>::instance()->DestroyInstance("scene", ctx);
    ScriptObject<ScriptLocalizedText>::instance()->DestroyInstance("localizedText", ctx);

    if (MFORCE::Common::MonitorConfig::Instance().HasMechanicalWheels())
    {
        ScriptObject<Revolve>::instance()->DestroyInstance("revolve", ctx);
    }

    ScriptObject<Game>::instance()->DestroyInstance("game", ctx);

    ScriptObject<BigWinController>::instance()->DestroyInstance("bigWinController", ctx);
    ScriptObject<BingoModeController>::instance()->DestroyInstance("BingoModeController", ctx);
    ScriptObject<HandpayController>::instance()->DestroyInstance("handpayController", ctx);
    ScriptObject<Hardware>::instance()->DestroyInstance("hardware", ctx);
    ScriptObject<CashoutController>::instance()->DestroyInstance("cashoutController", ctx);
    ScriptObject<UserMessageController>::instance()->DestroyInstance("userMessage", ctx);

    mpBigWinController = NULL;
    mpBingoModeController = NULL;
    mpHardware = NULL;
    mpAudio = NULL;
    mpHandpayController = NULL;
    mpCashoutController = NULL;
    mpUserMessageController = NULL;

    if (mRoot)
    {
        const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();
        MFORCE::STLExtensions::for_each(MFORCE::STLExtensions::MakeIseq(installedPlugins), std::bind(&OgreApplication::PluginOnContextTerminating, this, std::placeholders::_1, ctx));
    }

    for (auto listener : mpListeners)
    {
        listener->ScriptContextTerminating(ctx);
    }
}

void OgreApplication::onComponentTerminating(const std::string& ctxName, v8::Persistent<v8::Context>& ctx)
{
    v8::Locker locker;
    v8::HandleScope hs;
    v8::Context::Scope ctxTgt(ctx);

    ScriptObject<Audio>::instance()->DestroyInstance("audio", ctx);
    ScriptObject<Random>::instance()->DestroyInstance("random", ctx);
    ScriptObject<ScriptGraphics>::instance()->DestroyInstance("graphics", ctx);
    ScriptObject<Events>::instance()->DestroyInstance("events", ctx);
    ScriptObject<SceneManager>::instance()->DestroyInstance("scene", ctx);
    ScriptObject<ScriptLocalizedText>::instance()->DestroyInstance("localizedText", ctx);

    if (MFORCE::Common::MonitorConfig::Instance().HasMechanicalWheels())
    {
        ScriptObject<Revolve>::instance()->DestroyInstance("revolve", ctx);
    }

    if (mRoot)
    {
        const Ogre::Root::PluginInstanceList& installedPlugins = mRoot->getInstalledPlugins();
        MFORCE::STLExtensions::for_each(MFORCE::STLExtensions::MakeIseq(installedPlugins), std::bind(&OgreApplication::PluginOnContextTerminating, this, std::placeholders::_1, ctx));
    }

    for (auto listener : mpListeners)
    {
        listener->ScriptContextTerminating(ctx);
    }
}

void OgreApplication::onScriptCompiled(const std::string& ctxName, const std::string& resourcePath)
{
    updateIdleSceneLoading();
}

void OgreApplication::onThrowError(const std::string& errorMsg)
{
    MFORCE::ScreenLocks::ScreenLock::Instance().CriticalMemoryError(errorMsg);
}

void OgreApplication::windowOpened(RenderWindow* rw, unsigned int options)
{
    int windowIndex = (int)mWindows.size();

    mWindows[rw->getName()] = rw;
    if (mPrimaryWindow == nullptr)
    {
        // The first render window added is viewed by Ogre as the primary display window.
        // Note that mWindows.begin() does not return the first inserted render window,
        // so we have to store the primary render window manually.
        mPrimaryWindow = rw;
    }

    rw->setDeactivateOnFocusChange(false);

    Ogre::Camera* pCamera = mSceneMgr->createCamera(Poco::format("MasterUI_%s", rw->getName()));
    Ogre::Viewport* pViewport = rw->addViewport(pCamera, MASTER_OVERLAY_ZORDER + windowIndex);

    Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
    Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());

    Ogre::Real halfWidth = width * 0.5f;
    Ogre::Real halfHeight = height * 0.5f;

    float FOV = 45.0f;
    float fCameraDist = halfHeight / tan(Degree(FOV * 0.5f).valueRadians());

    pCamera->setPosition(Ogre::Vector3(halfWidth, -halfHeight, fCameraDist));

    pCamera->lookAt(Ogre::Vector3(halfWidth, -halfHeight, -300.0f));
    pCamera->setNearClipDistance(0.5f);
    pCamera->setAspectRatio(width / height);
    pCamera->setFOVy(Degree(FOV));

#if DEVELOPER 
    if (MFORCE::G2S::Cabinet::ShowEmbeddedMechReelsEmulator() && rw->getName() == "MAIN")
    {
        // Create memory render buffer for MAIN screen, it is used by LDM Emulator
        MemoryRenderBuffer* mainWindowRenderBuffer = new MemoryRenderBuffer("MAIN", Usage::Other, "RttTex");
        if (mainWindowRenderBuffer)
        {
            mainWindowRenderBuffer->Create(pCamera, static_cast<Ogre::uint>(width), static_cast<Ogre::uint>(height), this);
            mViewports["MAIN"+ MemoryRenderBuffer::GetPostfix()] = mainWindowRenderBuffer->GetViewportPtr();
            mMemoryRenderBuffers.push_back(mainWindowRenderBuffer);
        }
    }

    // Create memory render buffer for TOP screen, it is used F1 keyboard shortcut to grab a screenshot from
    if (MFORCE::Common::MonitorConfig::Instance().GetCabinetStyle() == MFORCE::Cabinet::CabinetStyleDynastyVueTopper)
    {
        if (rw->getName() == "VTOP1")
        {
            MemoryRenderBuffer* topWindowRenderBuffer = new MemoryRenderBuffer(rw->getName(), Usage::Screenshot);
            if (topWindowRenderBuffer)
            {
                topWindowRenderBuffer->Create(pCamera, static_cast<Ogre::uint>(width), static_cast<Ogre::uint>(height), this);
                mViewports[rw->getName() + MemoryRenderBuffer::GetPostfix()] = topWindowRenderBuffer->GetViewportPtr();
                mMemoryRenderBuffers.push_back(topWindowRenderBuffer);
            }
        }
    }
#endif

    mViewports[rw->getName()] = pViewport;
    if (mPrimaryViewport == nullptr)
    {
        // The first view port added is viewed by Ogre as the primary view port.
        // Note that mViewports.begin() does not return the first inserted view port,
        // so we have to store the primary view port manually.
        mPrimaryViewport = pViewport;
    }

    //Register as a Window listener
    WindowEventUtilities::addWindowEventListener(rw, this);

    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for (Ogre::Root::PluginInstanceList::const_iterator itr = plugins.begin();
        itr != plugins.end();
        ++itr)
    {
        ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(*itr);
        if (scriptablePlugin)
            scriptablePlugin->WindowOpened(rw);
    }

    for (auto listener : mpListeners)
    {
        listener->DeviceWindowOpened(rw->getName(), options);
    }

    //Set initial mouse clipping size
    windowResized(rw);
}

//Adjust mouse clipping area
void OgreApplication::windowResized(RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    for (auto listener : mpListeners)
    {
        listener->DeviceWindowResized(rw->getName(), width, height);
    }
}

void OgreApplication::windowClosed(RenderWindow* rw)
{
    for (auto listener : mpListeners)
    {
        listener->DeviceWindowClosed(rw->getName());
    }

    Ogre::Viewport* viewport= mViewports[rw->getName()];
    if (viewport)
    {
        Ogre::Camera* camera = viewport->getCamera();
        mSceneMgr->destroyCamera(camera);
    }
    rw->removeAllViewports();
    mViewports.erase(rw->getName());

    WindowEventUtilities::removeWindowEventListener(rw, this);

    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for (Ogre::Root::PluginInstanceList::const_iterator itr = plugins.begin();
        itr != plugins.end();
        ++itr)
    {
        ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(*itr);
        if (scriptablePlugin)
            scriptablePlugin->WindowClosed(rw);
    }

    if (rw == mPrimaryWindow && !mHibernating)
    {
        // Terminate the platform if primary window is closed
        DebugSupportLink& debugLink = DebugSupportLink::Instance();
        debugLink.LogMessage(Poco::format("Close request for window \"%s\". Terminating platform.", rw->getName()));
        Poco::Process::kill(Poco::Process::id());
    }
}

void OgreApplication::windowFocusChange(RenderWindow* rw)
{
    for (auto listener : mpListeners)
    {
        listener->DeviceWindowFocusChange(rw->getName());
    }

    // ogre singletons:
    // Does not create the instance on getSingleton() / getSingletonPtr(),
    // rather just has a pointer to track if one has been created already.
    // can be null when simpleUI shuts down.
    // Ogre::~Root destroys the overlay manager, and then unloads the plugins
    // which results in a windowFocusChange when the window is destroyed.
    CustomizedOverlayManager* om = CustomizedOverlayManager::getSingletonPtr();
    if (om)
    {
        om->releaseCapture();
    }
}

void MFORCE::GameEngine::OgreApplication::ActivateDemoMenu(const std::string& theme, const Json::Value& msg, const int reelIndex, const bool isUnityMGUI)
{
    if (mpDemoMenuUI)
        mpDemoMenuUI->Activate(theme, msg, reelIndex, isUnityMGUI);
}

void MFORCE::GameEngine::OgreApplication::DeactivateDemoMenu(bool forceUnload)
{
    if (IsDemoMenuActive())
        mpDemoMenuUI->Deactivate("", forceUnload);
}

bool MFORCE::GameEngine::OgreApplication::IsDemoMenuActive()
{
    return (mpDemoMenuUI && mpDemoMenuUI->IsActive());
}

void MFORCE::GameEngine::OgreApplication::AudioDebug(const Json::Value & msg)
{
#if DEVELOPER
    mpMasterUI->AudioDebugMessage(msg);
#endif // DEVELOPER
}

void MFORCE::GameEngine::OgreApplication::AudioDebugEQ(const Json::Value & msg)
{
#if DEVELOPER
    mpMasterUI->AudioEQMessage(msg);
#endif // DEVELOPER
}

void MFORCE::GameEngine::OgreApplication::FireAudioEvent(const Json::Value & msg)
{
    if (mpAudio)
    {
        mpAudio->fireOnSoundEvent(msg);
    }
}

void MFORCE::GameEngine::OgreApplication::AutoSnapshotCommand(const Json::Value& msg)
{
    std::string command = msg.get("command", std::string()).asString();

    switch (string_hash(command.c_str(), command.size())) {
    case "Toggle On/Off"_sh:
        mAutoSnapshot->toggle();
        break;
    case "Increase Interval"_sh:
        mAutoSnapshot->increaseInterval();
        break;
    case "Decrease Interval"_sh:
        mAutoSnapshot->decreaseInterval();
        break;
    case "Execute Snapshot"_sh:
        mAutoSnapshot->executeSnapshot();
        break;
    }
}

bool MFORCE::GameEngine::OgreApplication::HasRevolveWheel()
{
    return MFORCE::Common::MonitorConfig::Instance().HasMechanicalWheels();
}

void MFORCE::GameEngine::OgreApplication::ResetWheel()
{
    Json::Value revolveCommand = MFORCE::Common::RevolveMessaging::RevolveResetWheel();
    SendEvent(revolveCommand);
}

Ogre::RenderWindow* OgreApplication::getPrimaryRenderWindow()
{
    return mPrimaryWindow;
}

const std::string& OgreApplication::getPrimaryRenderWindowName()
{
    return mPrimaryWindow->getName();
}

RenderWindow* OgreApplication::getRenderWindow(const std::string& name)
{
    if (mWindows.find(name) != mWindows.end())
        return mWindows[name];
    else
        return NULL;
}

int OgreApplication::getNumWindows()
{
    return (int)mWindows.size();
}

const std::string OgreApplication::getActiveWindowName() const
{
#if defined( WIN32 )
    HWND hwnd = GetForegroundWindow();
    char name[255];
    ZeroMemory(name, sizeof(name));
    GetWindowText(hwnd, name, sizeof(name));
    return std::string(name);
#else
    return std::string("");
#endif
}

Ogre::Viewport* OgreApplication::getPrimaryViewport()
{
    return mPrimaryViewport;
}

const std::string& OgreApplication::getPrimaryViewportName()
{
    // The primary window name matches with the primary view port
    return mPrimaryWindow->getName();
}

Ogre::Viewport*	OgreApplication::getTopViewport(const std::string& windowName, bool useVirtualWindows)
{
    Ogre::Viewport* viewport = nullptr;

    // search virtual windows manager first if useVirtualWindows
    if ( useVirtualWindows )
    {
        viewport = mVirtualWindowManager.getViewport(windowName);
        if (viewport) return viewport;
    }

    if ( mViewports.find(windowName) != mViewports.end() )
    {
        viewport = mViewports[windowName];
    }

    return viewport;
}

void OgreApplication::InitializePluginScriptPrototypes(Ogre::Root::PluginInstanceList::value_type plugin)
{
    v8::Locker locker;
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
    {
        scriptablePlugin->Initialize(this);
        if (mpScriptEngine)
            scriptablePlugin->InitializeScriptPrototypes(*mpScriptEngine);
    }
}

void OgreApplication::PurgePluginScriptObjects(Ogre::Root::PluginInstanceList::value_type plugin)
{
    v8::Locker locker;
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->PurgeScriptObjects();
}

void OgreApplication::PluginOnPreGameLoad(Ogre::Root::PluginInstanceList::value_type plugin, const bool isTournament)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnPreGameLoad(isTournament);
}

void OgreApplication::PluginOnGameLoad(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnGameLoad();
}

void OgreApplication::PluginOnGameUnload(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnGameUnload();
}

void OgreApplication::PluginOnSetLanguage(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnSetLanguage(Localization::GetLanguage());
}

void OgreApplication::PluginOnActiveThemeIdUpdate(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnActiveThemeIdUpdate(mActiveGame.ThemeId);
}

void OgreApplication::DeletePluginScriptObjectSingleton(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->DeleteInstance();
}

void OgreApplication::PluginOnContextCreated(Ogre::Root::PluginInstanceList::value_type plugin, v8::Persistent<v8::Context>& ctx)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
    {
        scriptablePlugin->OnContextCreated(ctx);
    }
}

void OgreApplication::PluginOnContextTerminating(Ogre::Root::PluginInstanceList::value_type plugin, v8::Persistent<v8::Context>& ctx)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
    {
        scriptablePlugin->OnContextTerminating(ctx);
    }
}

void OgreApplication::PluginOnEnterTournamentMode(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnEnterTournamentMode();
}

void OgreApplication::PluginOnExitTournamentMode(Ogre::Root::PluginInstanceList::value_type plugin)
{
    ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
    if (scriptablePlugin)
        scriptablePlugin->OnExitTournamentMode();
}

void OgreApplication::StartGameScript()
{
    logSmallChunk(mActiveGame.ThemeId + " StartGameScript start ");
    if (!totalLoadTime.IsRunning())
        totalLoadTime.Start();
    scriptStartupTime.Start();

    poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), "StartGameScript");

    // Don't force GC if we have a multigame bundle, as we likely did on game exit.
    bool forceGC = mActiveBundle.empty();

    // terminate any emulator components 
    mpScriptEngine->TerminateGameScript(bind(&OgreApplication::PurgeScriptObjects, this), forceGC);

    logSmallChunk(mActiveGame.ThemeId + " TerminateGameScript ");

    NotificationManager::Instance().AddObserver(Poco::NObserver<OgreApplication, GameInPlayNotification>(*this, &OgreApplication::onGameInPlay));

    // show min required platform version for this game, only show once
    const MFORCE::SemanticVersion::Version& minPlatformVersion = Repository::Instance().GetRequiredPlatformVersion(mActiveGame.ThemeId);
    static std::string sMinPlatformVersion, sThemeName;

    if (sMinPlatformVersion != minPlatformVersion.ToString() || sThemeName != mActiveGame.ThemeName)
    {
        sMinPlatformVersion = minPlatformVersion.ToString();
        sThemeName = mActiveGame.ThemeName;
        poco_notice(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Game min required platform version = %s for %s", sMinPlatformVersion, sThemeName));
    }

    ScriptEngine::SetArgumentValidationAllowed(true);

    if (!mpScriptEngine || !mpScriptEngine->IsTerminated(mActiveGame.ThemeName))
        return;

    if (mLoadStatus != LOAD_COMPLETE)
        return;

    if (mGameFeatures.get() == NULL)
    {
        std::vector<std::string> nitroBonusThemeIds;
        std::copy(mNitroBonusThemes.begin(), mNitroBonusThemes.end(), std::back_inserter(nitroBonusThemeIds));

        mGameFeatures.reset(new GameFeatures(mActiveGame.ThemeId, nitroBonusThemeIds, MFORCE::G2S::LoadedPaytables::Instance()));
    }
    else
    {
        mGameFeatures.get()->CheckHelpScreenAttributes();
    }

    if (!mActiveGame.NitroBonusData.empty())
    {
        // Prevent rendering while we are manipulating resources
        OGRE_NAMED_MUTEX_SCOPED_LOCK

            if (Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(DLCMarqueeResourceGroupName))
            {
                Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(DLCMarqueeResourceGroupName);
            }
    }

    std::string gameroot = PathInfo::Instance()->GetGamePath(mActiveGame.ThemeId);
    Poco::Path gameRootPath(gameroot);
    gameRootPath.makeAbsolute();

    // The Bootstrap call to v8::Debug::EnableAgent with a "wait for connect" value of
    // 'true' empirically doesn't work.  See my posting on the v8-users mailing list/group
    // at http://groups.google.com/group/v8-users/browse_thread/thread/61740714ab5237f2.
    // We need to give it a kick in the butt by invoking a V8 debug break command.
    bool debugwait = (Poco::NumberParser::parseUnsigned(mOptions["debugwait"]) != 0);

    std::string contextName;
    std::string searchPath;
    std::string scriptName;

    logSmallChunk(mActiveGame.ThemeId + " Pre Context ");

    // Start component game scripts
    LoadComponentScripts(mActiveGame.ThemeId, gameRootPath.toString(), false);

    if (!DataLayer::Repository::Instance().GetGamePlayDataSource()->HasBaseComponent(mActiveGame.ThemeId))
    {
        /// This is a legacy, non-componentized game.
#if defined(DEVELOPER)
        try
#endif
        {
            contextName = mActiveGame.ThemeId;
            searchPath = gameRootPath.toString();
            scriptName = "main.js";

            mpScriptEngine->CreateContext(contextName, this, ScriptEngine::StartFileType_Base, debugwait, "");
            mpScriptEngine->StartFile(contextName, searchPath, scriptName);
        }
#if defined(DEVELOPER)
        catch (MFORCE::Common::ResourceException& e)
        {
            throw MFORCE::Foundation::ScriptException(contextName, searchPath, scriptName, e.displayText());
        }
#endif
    }

    logSmallChunk(mActiveGame.ThemeId + " Post context ");

    // start Nitro bonus scripts..
    if (!mActiveGame.NitroBonusData.empty())
    {
        std::string dlcPackageName;
#if defined(DEVELOPER)
        try
#endif
        {
            DLCManagerPlugin* pDLCManager = dynamic_cast<DLCManagerPlugin*>(getPluginByName(PackageManagerPluginName::DLCPackage));
            if (!pDLCManager)
            {
                throw Poco::NullPointerException("No DLCManagerPlugin available to process mActiveGame.NitroBonusData");
            }

            if (!pDLCManager->hostInitiatedNitroBonusUnload()) // Don't try to start the bonus if it has been unloaded at host's request. The EGM is in an unplayable state.
            {
                for (const auto& curBonusData : mActiveGame.NitroBonusData)
                {
                    std::string themeId = curBonusData.get("themeId", "").asString();
                    if (!themeId.empty())
                    {
                        dlcPackageName = DLCPrefix + themeId;
                        if (!pDLCManager->startNitroBonus(dlcPackageName))
                        {
                            throw Poco::DataFormatException(std::string("Unable to start NitroBonus context: ") + contextName);
                        }
                    }
                    else
                    {
                        throw Poco::DataFormatException("Incomplete mActiveGame.NitroBonusData");
                    }
                }
            }
        }
#if defined(DEVELOPER)
        catch (MFORCE::Common::ResourceException& e)
        {
            throw MFORCE::Foundation::ScriptException("StartGameScript", dlcPackageName, "", e.displayText());
        }
#endif
    }

    if (!mButtonModeActor)
        mButtonModeActor.assign(new ButtonModeActor(isBingoGame()));

    DispatchScriptEvent(&Events::mGameStartFunc);

    logSmallChunk(mActiveGame.ThemeId + " DispatchScript ");

#ifdef DEVELOPER
    injectPatsIntoJsContext(GetActiveThemeId());
#endif

    logSmallChunk(mActiveGame.ThemeId + " Pats ");

    Json::Value msg;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM,
        "GameScriptStarted",
        MFORCE::Messaging::Message::EVENT,
        msg);
    msg["isInSitAndGo"] = mIsInSitAndGoTransitionArc;
    SendEvent(msg);

    logSmallChunk(mActiveGame.ThemeId + " GameScriptStarted event sent ");
    smallChunkTime.Reset();

    // Hide the loading screen after we load the game scripts
    mpIdleScene->ShowLoadingScene(false);
    mpIdleScene->Unload();

    // Execute TE stitching package if necessary
    if (mActiveGame.IsTournamentPlay)
    {
        std::string stitchingType = DataLayer::Repository::Instance().GetGamePlayDataSource()->GetStitchingType(mActiveGame.ThemeId);
        if (MFORCE::G2S::Cabinet::IsLoadingStitchingPackageNeeded(stitchingType))
        {
            Poco::Path stitchingPackagePath;
            if (Common::PathInfo::Instance()->GetPath("TESTITCHING", stitchingPackagePath))
            {
                bool isTENowTheme = DataLayer::Repository::Instance().GetGamePlayDataSource()->IsTENowTheme(mActiveGame.ThemeId);
                
                Json::Value command;
                command["TEGameType"] = (isTENowTheme ? "TENow" : "TE");

                Json::Value packageMsg;
                MFORCE::Messaging::Message::InitializeMessage(GROUP_DLC, "DLCPackageRequest", MFORCE::Messaging::Message::EVENT, packageMsg);
                packageMsg[Json::StaticString("requestType")] = ePackageRequestType_Execute;
                packageMsg[Json::StaticString("extracted_path")] = stitchingPackagePath.makeFile().toString();
                packageMsg[Json::StaticString("command")] = command.toStyledString();
                SendEvent(packageMsg);
            }
        }
    }
}

void OgreApplication::StopGameScript()
{
    poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), "StopGameScript");
    TrackFrameRates(false);

    mGameTimeScale.setTimeScale(1.0f);

    // workaround ogre bug where shaders get stuck on
    Root::getSingleton().getRenderSystem()->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
    Root::getSingleton().getRenderSystem()->unbindGpuProgram(GPT_VERTEX_PROGRAM);
    Root::getSingleton().getRenderSystem()->unbindGpuProgram(GPT_GEOMETRY_PROGRAM);

    mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
    mSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

    evictManagedResources();
    NotificationManager::Instance().RemoveObserver(Poco::NObserver<OgreApplication, GameInPlayNotification>(*this, &OgreApplication::onGameInPlay));

    if (!mpScriptEngine)
        return;

    // notify game scripts that they are being terminated
    DispatchScriptEvent(&Events::mGameTerminateFunc);

    // terminate any game, GameComponent, and NitroBonus V8 contexts and cleanup remaining native script objects
    mpScriptEngine->TerminateGameScript(bind(&OgreApplication::PurgeScriptObjects, this), true);

    if (!mActiveGame.NitroBonusData.empty())
    {
        std::string dlcPackageName;
#if defined(DEVELOPER)
        try
#endif
        {
            DLCManagerPlugin* pDLCManager = dynamic_cast<DLCManagerPlugin*>(getPluginByName(PackageManagerPluginName::DLCPackage));
            if (!pDLCManager)
            {
                throw Poco::NullPointerException("No DLCManagerPlugin available to process mActiveGame.NitroBonusData");
            }

            for (const auto& curBonusData : mActiveGame.NitroBonusData)
            {
                std::string themeId = curBonusData.get("themeId", "").asString();
                if (!themeId.empty())
                {
                    dlcPackageName = DLCPrefix + themeId;
                    pDLCManager->stopNitroBonus(dlcPackageName);
                }
                else
                {
                    throw Poco::DataFormatException("Incomplete mActiveGame.NitroBonusData");
                }
            }
        }
#if defined(DEVELOPER)
        catch (MFORCE::Common::ResourceException& e)
        {
            throw MFORCE::Foundation::ScriptException("StopGameScript", dlcPackageName, "", e.displayText());
        }
#endif
    }

    Json::Value msg;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM,
        "GameScriptStopped",
        MFORCE::Messaging::Message::EVENT,
        msg);
    msg["isInSitAndGo"] = mIsInSitAndGoTransitionArc;
    SendEvent(msg);

    // nothing more to do if there is no active theme
    if (mActiveGame.ThemeName.empty())
        return;

    // this should really be done on the game script side (somewhere via onGameTerminate),
    // but released games don't do it.
    if (mpHandpayController)
    {
        mpHandpayController->fireOnHandpayClear();
    }

    // Restore Core overlay for cashout message.
    if (mpCashoutController)
    {
        mpCashoutController->resetToDefaults();
    }

    // shouldn't there be a more generic mechanism for this?
    BingoDisplayPlugin* bingoDisplayPlugin = (BingoDisplayPlugin*)(getPluginByName("Plugin_BingoDisplay"));
    if (bingoDisplayPlugin)
    {
        bingoDisplayPlugin->HideBingoPaytables();
    }

    // clear compositor chains since starting the game script will set these up again
    CompositorDirector::getSingleton().clearCompositorChains();

    // game may have added additional viewports, so
    // reset clearing on the main viewport
    Ogre::Viewport* pViewport = getTopViewport("MAIN");
    if (pViewport)
    {
        pViewport->setClearEveryFrame(true);
    }

    if (mGameFeatures.get() != NULL)
        mGameFeatures.get()->ResetFX();

    Json::Value stopAllSounds;
    Message::InitializeMessage(GROUP_AUDIO, "StopCueCommand", Message::EVENT, stopAllSounds);
    stopAllSounds["cueName"] = ""; // all cues
    stopAllSounds["soundBankname"] = ""; // all sound banks
    stopAllSounds["immediate"] = true;
    SendEvent(stopAllSounds);

    mButtonModeActor.assign(NULL);

    if (!mActiveGame.NitroBonusData.empty())
    {
        // Prevent rendering while we are manipulating resources
        OGRE_NAMED_MUTEX_SCOPED_LOCK

            if (Ogre::ResourceGroupManager::getSingleton().resourceGroupExists(DLCMarqueeResourceGroupName))
            {
                Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(DLCMarqueeResourceGroupName);
            }
    }

    // Note that system emulators are not restarted if the active theme is being unloaded, 
    // or there is no active theme.  This is due to two reasons:
    // 1. Ogre resources will be unloaded, including those in use by the emulators even if running.
    // 2. A limitation of the current MechReelsEmulator which requires an active theme to define reel symbols.  
    //    This limitation can be relaxed if the platform could provide default reelstrips and reel symbols. 
    if (mLoadStatus == LOAD_UNLOADING)
        return;

    // There is an active theme, and it is not being unloaded...
    // Restart any system emulator components
    bool debugwait = (Poco::NumberParser::parseUnsigned(mOptions["debugwait"]) != 0);

    DataLayer::ThemeComponentList compList = GetEmulatorComponents(mActiveGame.ThemeId);

    for (DataLayer::ThemeComponentList::const_iterator i = compList.begin(); i != compList.end(); ++i)
    {
        const DataLayer::ThemeComponent& gtc = *i;
        std::string contextName = gtc.Name;

        mpScriptEngine->CreateContext(contextName,
            this,
            ScriptEngine::StartFileType_Component,
            debugwait, gtc.Args);

        Poco::Path componentPath = PathInfo::Instance()->ParseTaggedPathString(gtc.Path, "/");

        mpOgreLog->logMessage(Poco::format("Loading emulator Component %s from location %s", contextName, componentPath.toString()));
        mpScriptEngine->StartFile(contextName, componentPath.toString(), "main.js");
    }

}

void OgreApplication::PurgeScriptObjects()
{
    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PurgePluginScriptObjects), this));

    ScriptObject<Animation>::instance()->Purge();
    ScriptObject<AnimationScript>::instance()->Purge();
    ScriptObject<AnticipationController>::instance()->Purge();
    ScriptObject<Audio>::instance()->Purge();
    ScriptObject<AutopickTimer>::instance()->Purge();
    ScriptObject<BangupController>::instance()->Purge();
    ScriptObject<BigWinController>::instance()->Purge();
    ScriptObject<BingoModeController>::instance()->Purge();
    ScriptObject<Camera>::instance()->Purge();
    ScriptObject<Color>::instance()->Purge();
    ScriptObject<Design>::instance()->Purge();
    ScriptObject<Events>::instance()->Purge();
    ScriptObject<Game>::instance()->Purge();
    ScriptObject<GameWindow>::instance()->Purge();
    ScriptObject<ScriptGraphics>::instance()->Purge();
    ScriptObject<HandpayController>::instance()->Purge();
    ScriptObject<Hardware>::instance()->Purge();
    ScriptObject<CashoutController>::instance()->Purge();
    ScriptObject<Light>::instance()->Purge();
    ScriptObject<LuckyDust>::instance()->Purge();
    ScriptObject<ScriptMathData>::instance()->Purge();
    ScriptObject<MouseFollower>::instance()->Purge();
    ScriptObject<OverlayGroup>::instance()->Purge();
    ScriptObject<ScriptMessageBus>::instance()->Purge();
    ScriptObject<Millicent>::instance()->Purge();
    ScriptObject<MusicalBangup>::instance()->Purge();
    ScriptObject<PressToChangeController>::instance()->Purge();
    ScriptObject<Random>::instance()->Purge();
    ScriptObject<Revolve>::instance()->Purge();
    ScriptObject<SceneManager>::instance()->Purge();
    ScriptObject<ScriptAttractMessageCycler>::instance()->Purge();
    ScriptObject<ScriptBannersManager>::instance()->Purge();
    ScriptObject<ScriptFxManager>::instance()->Purge();
    ScriptObject<ScriptGameOverController>::instance()->Purge();
    ScriptObject<ScriptLocalizedText>::instance()->Purge();
    ScriptObject<ScriptLogger>::instance()->Purge();
    ScriptObject<ScriptMechReelsManager>::instance()->Purge();
    ScriptObject<ScriptOgreSceneNode>::instance()->Purge();
    ScriptObject<ScriptOgreScreenshot>::instance()->Purge();
    ScriptObject<ScriptOgreSlotGame>::instance()->Purge();
    ScriptObject<ScriptPerformanceTimer>::instance()->Purge();
    ScriptObject<ScriptPlayerBalanceDisplay>::instance()->Purge();
    ScriptObject<ScriptReelsManager>::instance()->Purge();
    ScriptObject<ScriptRenderTexture>::instance()->Purge();
    ScriptObject<ScriptSplineFollower>::instance()->Purge();
    ScriptObject<UserMessageController>::instance()->Purge();
    ScriptObject<Vector2>::instance()->Purge();
    ScriptObject<Vector3>::instance()->Purge();
    ScriptObject<Vector4>::instance()->Purge();
    ScriptObject<Viewport>::instance()->Purge();
    ScriptObject<ScriptOgreComponentGame>::instance()->Purge();
    ScriptObject<ScriptOgreBoltOnBonusGame>::instance()->Purge();
    ScriptObject<Pats>::instance()->Purge();
    ScriptObject<ScriptOgreLanguageManager>::instance()->Purge();
    ScriptObject<ScriptTENowEntry>::instance()->Purge();

    WidgetScriptObject<Sprite, SpriteOverlayElement>::instance()->Purge();
}

void OgreApplication::SendEvent(const Json::Value& msg)
{
    for (auto listener : mpListeners)
    {
        listener->ScriptSendEvent(msg);
    }

    /// Generate notification messages for outgoing slot events
    std::string msgType = msg.get("typeName", "").asString();

    if (!msgType.empty())
    {
        if (msgType == "ResultDisplayedEvent")
        {
            DispatchScriptEvent(&Events::mOnResultDisplayedFunc);
            processPatsMessages(msg);
        }
        else if (msgType == "AwardDisplayedEvent")
        {
            DispatchScriptEvent(&Events::mOnAwardDisplayedFunc);
            processPatsMessages(msg);
        }
    }
}

void OgreApplication::SendReloadAudioEvent()
{
    Json::Value reloadAudio;
    Message::InitializeMessage(GROUP_SYSTEM, "ReloadAudioEvent", Message::EVENT, reloadAudio);
    reloadAudio["themeName"] = mActiveGame.ThemeName;
    reloadAudio["themeId"] = mActiveGame.ThemeId;
    reloadAudio["gamePlayId"] = mActiveGame.Id;
    reloadAudio["boltOnBonusData"] = mActiveGame.NitroBonusData;
    SendEvent(reloadAudio);
}

void OgreApplication::DispatchScriptEvent(v8FuncProp prop, v8::Handle<v8::Value> args[], int argc, const std::string& themeId)
{
    v8::Locker locker;
    v8::HandleScope hs;

    const ScriptEngine::ContextMap& ctxMap = mpScriptEngine->GetContextMap();
    for (ScriptEngine::ContextMap::const_iterator i = ctxMap.begin(); i != ctxMap.end(); ++i)
    {
        const v8::Persistent< v8::Context >& ctx = i->second;
        v8::Context::Scope cs(ctx);
        Events* pEvts = ScriptObject<Events>::UnwrapGlobalObject(ctx, "events");
        if (pEvts)
        {
            const std::string& eventsContextName = pEvts->GetContextName();

            std::string eventsThemeId;
            bool bIsMappedToTheme = ConvertContextNameToThemeId(eventsContextName, eventsThemeId);

            // only if context is theme-less (ie: global listener) or tied to this specific themeId do we actually dispatch the event..
            if (!bIsMappedToTheme || (eventsThemeId == themeId))
            {
                MFORCEScript::v8CallbackFunction& func = pEvts->*prop;
                if (!func.IsEmpty())
                {
                    if (argc != 0)
                    {
                        func.Call(args, argc);
                    }
                    else
                    {
                        func.Call();
                    }
                }
            }
        }
    }
}

bool OgreApplication::ConvertContextNameToThemeId(const std::string& contextName, std::string& out_themeId) const
{
    ContextNameToThemeIdMap::const_iterator itr = mContextNameToThemeIdMap.find(contextName);
    if (itr != mContextNameToThemeIdMap.end())
    {
        out_themeId = itr->second;
        return true;
    }
    else
    {
        out_themeId = "";
        return false;
    }
}

void OgreApplication::DispatchScriptEvent(v8FuncProp prop, v8::Handle<v8::Value> args[], int argc)
{
    v8::Locker locker;
    v8::HandleScope hs;

    const ScriptEngine::ContextMap& ctxMap = mpScriptEngine->GetContextMap();
    for (ScriptEngine::ContextMap::const_iterator i = ctxMap.begin(); i != ctxMap.end(); ++i)
    {
        const v8::Persistent< v8::Context >& ctx = i->second;
        v8::Context::Scope cs(ctx);
        Events* pEvts = ScriptObject<Events>::UnwrapGlobalObject(ctx, "events");
        if (pEvts)
        {
            MFORCEScript::v8CallbackFunction& func = pEvts->*prop;
            if (!func.IsEmpty())
            {
                if (argc != 0)
                {
                    func.Call(args, argc);
                }
                else
                {
                    func.Call();
                }
            }
        }
    }
}

void OgreApplication::DispatchScriptEvent(v8FuncProp prop)
{
    v8::Locker locker;
    v8::HandleScope hs;

    v8::Handle<v8::Value> dummy[1];
    DispatchScriptEvent(prop, dummy, 0);
}

void OgreApplication::prepareForPreload()
{
    mLoadStatus = LOAD_DISK;
    CustomizedOverlayManager::getSingletonPtr()->enableOverlaySorting(false);
    setTargetFPS(15);
    if (!mShowSitAndGoLoadingScene)
    {
        mpIdleScene->ShowLoadingOverlays(true);
    }
    mpIdleScene->ShowLoadingScene(true);
}

void OgreApplication::loadBundle(const std::string& bundleId)
{
    // We want the bundle to take control of the MarqueeEmulator component
    linkComponentToThemeId("MarqueeEmulator", bundleId);

    mActiveBundle = bundleId;

    prepareForPreload();
    setLoadMessage(std::string("Multigame Bundle"));
    poco_notice(Poco::Logger::get("gameengine"), Poco::format("Loading bundle %s (%s) ...", MultiGameManager::getInstance().getBundleName(mActiveBundle),
        MultiGameManager::getInstance().getBundleInfo(mActiveBundle)->mBundleVersion));

    mPreloadManager.preloadBundle(bundleId, &mLoadDependencyMgr);

    preloadButtonPanel();

#ifdef DEVELOPER
    injectPatsIntoJsContext(bundleId);
#endif

    DeactivateDemoMenu();
    mpMasterUI->DeactivateTechScreen();
}


void OgreApplication::unloadBundle(const std::string& bundleId)
{
    // Don't want to show during sit and go theme loading
    if (!mShowSitAndGoLoadingScene)
    {
        mpMasterUI->DisplayUnavailableScreens(true);
    }

    unloadButtonPanel();
    poco_notice(Poco::Logger::get("gameengine"), Poco::format("Unloading bundle %s ...", MultiGameManager::getInstance().getBundleName(mActiveBundle)));
    mPreloadManager.unloadBundle(bundleId);
    mpMasterUI->EnableDemoMenuButton(false);
}



void OgreApplication::preloadTheme(const std::string& themeId)
{
    prepareForPreload();
    setLoadMessage(std::string("Preload Theme"));

    mPreloadManager.preloadTheme(themeId, &mLoadDependencyMgr);

    preloadButtonPanel();

    DeactivateDemoMenu();
    mpMasterUI->DeactivateTechScreen();
}

void OgreApplication::preloadButtonPanel()
{
    DataLayer::ThemeComponentList componentList;
    const std::string emptyThemeId = "";
    AddComponentForMonitor("ButtonPanel", "BUTTON_PANEL", emptyThemeId, componentList);
    for (const auto& component : componentList)
    {
        mPreloadManager.preloadComponent(component.Name, component.Path);
    }
}

void OgreApplication::unloadButtonPanel()
{
    DataLayer::ThemeComponentList componentList;
    const std::string emptyThemeId = "";
    AddComponentForMonitor("ButtonPanel", "BUTTON_PANEL", emptyThemeId, componentList);
    for (const auto& component : componentList)
    {
        mPreloadManager.unloadPreloadedComponent(component.Name);
    }
}

void OgreApplication::startGame(const MFORCE::GamePlay::Game& gameToStart)
{
    scriptStartupTime.Reset();
    totalLoadTime.Reset();
    totalLoadTime.Start();
    startSmallChunk();

    if (mActiveGame.Id != 0)
    {
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::startGame() called with theme '%s' but appears to still have active theme '%s'.", gameToStart.ThemeId, mActiveGame.ThemeId));
        return;
    }

    // Mark this theme as the owner of these components if something doesn't already own it
    linkComponentToThemeIdIfEmpty("ButtonPanel", gameToStart.ThemeId);
    linkComponentToThemeIdIfEmpty("MechReelsEmulator", gameToStart.ThemeId);
    linkComponentToThemeIdIfEmpty("MarqueeEmulator", gameToStart.ThemeId);

    // Save the current window resolutions in case the game overrides it
    SaveWindowResolutions();

    // check to see if game wants to override any window resolutions
    WindowMap::const_iterator itWindows = mWindows.begin();
    MonitorConfig& monitorConfig = MonitorConfig::Instance();
    if (monitorConfig.GetFullscreen()) //Don't adjust resolution on windowed.
    {
        auto monitors = monitorConfig.GetMonitors();
        while (itWindows != mWindows.end())
        {
            const std::string& windowName = (*itWindows).first;
            try
            {
                // get the resolution the game wants
                unsigned int gameWidth = 0;
                unsigned int gameHeight = 0;
                std::string gameResolution = "";
                DataLayer::Repository::Instance().GetGamePlayDataSource()->GetMonitorResolution(gameToStart.ThemeId, windowName, gameWidth, gameHeight, gameResolution);

                // check to see if resolution specification is allowed
                if (monitorConfig.IsAllowResolutionChange())
                {
                    // get the current resolution
                    Ogre::RenderWindow* window = (*itWindows).second;
                    const unsigned int windowWidth = window->getWidth();
                    const unsigned int windowHeight = window->getHeight();

                    // if they are different, change to game's resolution
                    if (windowWidth != gameWidth || windowHeight != gameHeight)
                    {
                        ChangeWindowResolution(window, gameWidth, gameHeight);
                    }
                }
            }
            catch (GameManifestException& /*e*/)
            {
                // game does not specify a resolution for this monitor
            }

            ++itWindows;
        }
    }

    if (gameToStart.IsTournamentPlay)
    {
        // TE games: disable MAIN window multitouch if it is available
        DisableWindowMultitouch("MAIN");
    }
    else
    {
        // in-rev games: enable MAIN window multitouch if it is available
        EnableWindowMultitouch("MAIN");
    }

    logSmallChunk(gameToStart.ThemeId + " Window Resolution ");

    std::string stitchingType = DataLayer::Repository::Instance().GetGamePlayDataSource()->GetStitchingType(gameToStart.ThemeId);
    if (gameToStart.IsTournamentPlay && (MFORCE::G2S::Cabinet::IsStitchingFromPackageNeeded(stitchingType) || MFORCE::G2S::Cabinet::IsPlatformStitchingNeeded(stitchingType)))
    {
        createStitchingViewports(stitchingType);
    }

    mActiveGame = gameToStart;
    mbSwitchLanguage = false;

    // Always register primary themeId as a load dependency.
    mLoadDependencyMgr.registerDependency(mActiveGame.ThemeId);

    poco_notice(Poco::Logger::get("gameengine"), Poco::format("Loading game %s (%s) ...", mActiveGame.ThemeName, mActiveGame.ThemeVersion));
    mCentralType = DataLayer::PersistentStoreFactory::GetAttributePersistence("gamePlay.centralType")->get();

    std::string gamesPath = PathInfo::Instance()->GetGamePath(mActiveGame.ThemeId);
    Poco::Path gameRoot(gamesPath);
    gameRoot.makeDirectory();

    resetLoadTimer();
    mLoadStatus = LOAD_DISK;

    // throttle the main thread during load to give the disk i/o more cpu.
    setTargetFPS(15);
    // notify gameLoadPending to allow for any pre-processing
    NotificationManager::Instance().PostNotification(new MFORCE::GamePlay::GameLoadPendingNotification(mActiveGame));

    CustomizedOverlayManager::getSingletonPtr()->enableOverlaySorting(true);

    if (!mShowSitAndGoLoadingScene)
    {
        mpIdleScene->SetActiveThemeId(mActiveGame.ThemeId);

        if (mActiveBundle.empty())
        {
            mpIdleScene->ShowLoadingOverlays(true);
            setLoadMessage(std::string("Resources"));
        }
        else
        {
            // When changing games in a bundle, we don't want to show any of the loading messages
            mpIdleScene->ShowLoadingOverlays(false);
        }
    }

    mpIdleScene->ShowLoadingScene(true);
    CustomizedOverlayManager::getSingletonPtr()->updateOverlayOrder();
    mRoot->renderOneFrame(0);
    CustomizedOverlayManager::getSingletonPtr()->enableOverlaySorting(false);

    logSmallChunk(gameToStart.ThemeId + " LoadScreenShown ");

    // Preload scripts
    try
    {
        const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
        for (Ogre::Root::PluginInstanceList::const_iterator itr = plugins.begin();
            itr != plugins.end();
            ++itr)
        {
            PluginOnPreGameLoad(*itr, mActiveGame.IsTournamentPlay);
        }
    }
    catch (Poco::Exception& e)
    {
        mpOgreLog->logMessage("Script Load Failure", LML_CRITICAL);
        mpOgreLog->logMessage(e.message().c_str(), LML_CRITICAL);
    }

    logSmallChunk(gameToStart.ThemeId + " PluginOnPreGameLoad ");

    // set atlas resources to be loaded into game atlas
    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
    if (pAtlasPlugin)
    {
        pAtlasPlugin->setCurrentTextureAtlas("Game");
    }

    logSmallChunk(gameToStart.ThemeId + " Texture Atlas ");

    // Initialize localization for system components (must assume no prize generator mathscript).  
    // Game component localization is initialized in the Config module (so localization is 
    // guaranteed to be initialized for both game engine and prize generator).
    std::string language = Localization::GetLanguage();

    ResourceCatalog& resourceCatalog = OgreResources::Instance()->GetResourceCatalog();

    DataLayer::ThemeComponentList compList = GetSystemComponents(gameToStart.ThemeId);
    for (DataLayer::ThemeComponentList::const_iterator it = compList.begin(); it != compList.end(); ++it)
    {
        Poco::Path componentPath = PathInfo::Instance()->ParseTaggedPathString(it->Path, gameRoot.toString());
        componentPath.makeDirectory();
        Poco::Path locPath(componentPath);
        locPath.pushDirectory("i18n");
        locPath.pushDirectory(language);

        resourceCatalog.AddResourceLocation(componentPath.toString(), "i18n/**");
        try
        {
            MFORCE::Common::FileEntryCollection i18nFileEntries;
            resourceCatalog.FindResources(locPath, "LocalizedTextData.xml", i18nFileEntries);
            if (!i18nFileEntries.empty())
            {
                std::auto_ptr<std::istream> localizationInput = resourceCatalog.LoadResource(i18nFileEntries.begin()->Path());
                Localizer themeLocalizer;
                themeLocalizer.SetLanguage(Localization::GetLanguage());
                themeLocalizer.SetMonetaryLanguage(Localization::GetMonetaryLanguage());

                LocalizerBuilder::BuildLocalizer(*localizationInput, themeLocalizer);
                Localization::Merge(themeLocalizer);
            }
            else
                poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("No localization data found for component [%s]", it->Name));
        }
        catch (const ResourceException& ex)
        {
            poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("No localization data loaded for component [%s]: %s", it->Name, ex.displayText()));
        }
        catch (const LocalizerBuilderException& ex)
        {
            poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("No localization data loaded for component [%s]: %s", it->Name, ex.displayText()));
        }
    }

    logSmallChunk(gameToStart.ThemeId + " System Components ");

    if (mActiveGame.IsTournamentPlay)
    {
        // For stitched TE games, still use platform provided marquee resources regardless
        bool hasGameMarqueeResources = MFORCE::Cabinet::CabinetProxy::Instance().ThemeHasMarqueeResources(mActiveGame.ThemeId);
        if (MFORCE::G2S::Cabinet::IsLoadingCabinetTournamentResourceNeeded(stitchingType) || !hasGameMarqueeResources)
        {
            poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), "Tournevent Theme: " + mActiveGame.ThemeId + " use default Tournevent Marquee Resources");
            OgreResources::Instance()->RegisterTournamentMarqueeResourceGroup(mActiveGame.ThemeId);
        }
        if (MFORCE::G2S::Cabinet::IsStitchingFromPackageNeeded(stitchingType) && hasGameMarqueeResources)
        {
            // use platform to control TE marquee lighting like count down
            MFORCE::Cabinet::CabinetProxy::Instance().UnRegisterThemeWithMarqueeResources(mActiveGame.ThemeId);
        }
    }

    logSmallChunk(gameToStart.ThemeId + " Marquee ");

    updateIdleSceneLoading();

    //If bundle is active or a langauge toggle game Override Stream Setting for all Binks
    if (!mActiveBundle.empty() || DataLayer::Repository::Instance().GetGamePlayDataSource()->GetIsLanguageToggleForTheme(gameToStart.ThemeId))
    {
        auto bink_plugin = (BinkWidgetPlugin*)getPluginByName("Plugin_BinkWidget");
        if (bink_plugin)
        {
            bink_plugin->OverrideStreamSetting(true);
        }
    }

    // Load game resources
    OgreResources::Instance()->LoadResources(mActiveGame.ThemeId, GetComponentsToLoad(gameToStart.ThemeId));

    updateIdleSceneLoading();

    logSmallChunk(gameToStart.ThemeId + " LoadResources ");


    if (mActiveGame.IsTournamentPlay)
    {
        mIsTENowActive = DataLayer::Repository::Instance().GetGamePlayDataSource()->IsTENowTheme(mActiveGame.ThemeId);
        if (mIsTENowActive)
        {
            loadTransientPlugin("Plugin_TournEventNow");
        }
    }

    // Check for any bolt-on-bonus games that want to load with this theme.
    if (!mActiveGame.NitroBonusData.empty())
    {
        OgreResources::Instance()->RegisterDLCMarqueeResourceGroup();

        // Package paths are stored in a JSON array.
        for (const auto& curBonusData : mActiveGame.NitroBonusData)
        {
            // The themeId of the bonus must be registered with the render queue controller.
            const std::string& themeId = curBonusData.get("themeId", "").asString();
            if (!themeId.empty())
            {
                // Only allocate the render queue controller if we have a valid bolt-on-bonus.
                if (!mpRenderQueueController.get())
                {
                    mpRenderQueueController = std::make_unique<RenderQueueController>(mSceneMgr, mActiveGame.ThemeId);
                }

                // Create a new render queue group for this bolt-on-bonus.
                mpRenderQueueController->addQueueForTheme(themeId);

                // Cache this themeId for lookup.
                mNitroBonusThemes.insert(themeId);

                // If this package is already authenticated, enqueue it for loading after primary game is loaded.
                const bool authenticated = curBonusData.get("authenticated", false).asBool();
                if (authenticated)
                {
                    const std::string& path = curBonusData.get("path", "").asString();
                    if (!path.empty())
                    {
                        mPendingNitroBonuses.push(path);
                    }
                }
            }
        }
    }

    logSmallChunk(gameToStart.ThemeId + " Nitro Bonuses ");

    // assign system components to reserved render queue, so that they'll keep rendering when Bolt-on-Bonus becomes active theme..
    static const std::vector<Ogre::String> SystemComponentOverlayNames =
    {
        "MarqueeEmulatorOverlay",
        "MechReelsEmulatorOverlay",
        "MechReelsEmulator.Reels",
        "MechReelsEmulatorPCS.Reels",
        "ReelBackgroundOverlayPCS",
        "ReelBackgroundOverlayPCS1",
        "ReelBackLightsOverlayPCS",
        "ReelColumnLightsOverlayPCS",
        "ReelFrontLightsOverlayPCS"
    };

    for (const auto& curOverlayName : SystemComponentOverlayNames)
    {
        Ogre::Overlay* pSystemOverlay = GraphicUtil::getOverlay(curOverlayName);
        if (pSystemOverlay)
        {
            RenderQueueController::SetOverlayChildrenRenderQueue(pSystemOverlay, RenderQueueController::GLOBAL_FOREGROUND);
        }
    }

    logSmallChunk(gameToStart.ThemeId + " OverlayChildrenRenderQueue ");

    // NOTE: we don't actually start the game (script) here, as we need to
    // wait for all the assets to load... (See texturesPrepared()).
    DeactivateDemoMenu();
    mpMasterUI->DeactivateTechScreen();

    initializeActiveThemes();

    logSmallChunk(gameToStart.ThemeId + " Initialized ");
}

void OgreApplication::initializeActiveThemes()
{
    mContextNameToThemeIdMap.clear();

    if (mActiveGame.Id == 0)
    {
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::initializeActiveThemes() called with invalid mActiveGame.");
        return;
    }

    std::string primaryThemeID = mActiveGame.ThemeId;
    mContextNameToThemeIdMap[primaryThemeID] = primaryThemeID;

    // map theme-related GameComponent contexts to primaryThemeID..
    DataLayer::ThemeComponentList themeComponents = DataLayer::Repository::Instance().GetGamePlayDataSource()->GetThemeComponents(primaryThemeID);
    for (const auto& curThemeComponent : themeComponents)
    {
        mContextNameToThemeIdMap[curThemeComponent.Name] = primaryThemeID;
    }

    // register NitroBonus contexts..	
    for (const auto& curBonusData : mActiveGame.NitroBonusData)
    {
        const bool authenticated = curBonusData.get("authenticated", false).asBool();

        // Only register if the package is already authenticated, otherwise it will be registered
        // once we receive authentication.
        if (authenticated)
        {
            const std::string& nitroBonusThemeId = curBonusData.get("themeId", "").asString();
            const std::string& nitroBonusContext = curBonusData.get("contextName", "").asString();
            if (!nitroBonusThemeId.empty())
            {
                mContextNameToThemeIdMap[nitroBonusContext] = nitroBonusThemeId;
            }
        }
    }

    UnifiedMessageBroadcast::instance().setContextNameToThemeIdMap(mContextNameToThemeIdMap);
}

void OgreApplication::logLoadedResources()
{
    // Prevent rendering while we are manipulating resources
    OGRE_NAMED_MUTEX_SCOPED_LOCK

        Poco::Logger& logger = Poco::Logger::get(MODULENAME_GAMEENGINE);
    if (logger.debug())
    {
        Ogre::StringVector grps = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
        poco_debug(logger, "Resource Group Count: " + Poco::NumberFormatter::format(grps.size()));

        for (int grpIdx = 0; grpIdx < grps.size(); grpIdx++)
        {
            String grpName = grps[grpIdx];
            StringVectorPtr resNames = Ogre::ResourceGroupManager::getSingleton().listResourceNames(grpName);
            std::string logline = "\n\tGroup Name: " + grpName + " ( " + Poco::NumberFormatter::format(resNames->size()) + " entries )";
            for (int resIdx = 0; resIdx < resNames->size(); resIdx++)
            {
                String resName = (*resNames)[resIdx];
                logline += "\n\t\tResource Name: " + resName;
            }
            poco_debug(logger, logline);
        }
        std::string logline = "\n\nTexture Sizes:";
        Ogre::TextureManager::ResourceMapIterator it = Ogre::TextureManager::getSingleton().getResourceIterator();
        size_t total = 0;
        while (it.hasMoreElements())
        {
            Ogre::TexturePtr texture = it.getNext().staticCast<Ogre::Texture>();
            std::string textureName = texture->getName();
            int usage = texture->getUsage();
            size_t size = texture->getNumFaces() * Ogre::PixelUtil::getMemorySize(texture->getWidth(),
                texture->getHeight(), texture->getDepth(), texture->getFormat());
            logline += "\n\t[" + Poco::NumberFormatter::format(size / 1024) + "k] " + textureName;
            if (usage & Ogre::TU_STATIC)
                logline += " STATIC";
            if (usage & Ogre::TU_DYNAMIC)
                logline += " DYNAMIC";
            if (usage & Ogre::TU_WRITE_ONLY)
                logline += " WRITEONLY";
            if (usage & HardwareBuffer::HBU_DISCARDABLE)
                logline += " DISCARDABLE";
            if (usage & Ogre::TU_AUTOMIPMAP)
                logline += " AUTOMIPMAP";
            if (usage & Ogre::TU_RENDERTARGET)
                logline += " RENDERTARGET";
            total += size;
        }
        logline += "\n\t[" + Poco::NumberFormatter::format(total / 1024) + "k] TOTAL";
        poco_debug(logger, logline);
    }
}

void OgreApplication::stopGame(bool loadIdle)
{
    if (!mActiveGame.ThemeId.empty())
    {
        // Before we unload the game assets, first render a courtesy frame for the app...
        //  This is important for something like multi-game, so if we're switching to MGUI we would want to show it immediately
        mRoot->renderOneFrame();

        mLoadDependencyMgr.setDependencyLoaded(mActiveGame.ThemeId, false);
        mLoadDependencyMgr.unregisterDependency(mActiveGame.ThemeId);

        mpMasterUI->DeactivateGaffingUI();
        mpMasterUI->ResetAnimationStats();
        mCurrentReelLayout.clear();

        poco_notice(Poco::Logger::get("gameengine"), Poco::format("Unloading game %s ...", mActiveGame.ThemeName));
        mpOgreLog->logMessage(String("Stopping Game: " + mActiveGame.ThemeName));

        MathInfo::instance().reset();
        mTextFieldActors.clear();

        // terminate background loading
        mRoot->getWorkQueue()->abortAllRequests();

        // Don't want to show on bundle game switch and during sit and go theme loading
        if (mActiveBundle.empty() && !mShowSitAndGoLoadingScene)
        {
            mpMasterUI->DisplayUnavailableScreens(true);
        }

        mLoadStatus = LOAD_UNLOADING;

        StopGameScript();


        // If bundle is running, no need to stop all marquee scripts. 
        // It is handled in MGUI package request.
        if (mActiveBundle.empty())
        {
            EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
            egmPlugin->StopGameMarqueeScripts();
        }

        mGameFeatures.reset();

        if (mIsInSitAndGoTransitionArc)
        {
            if (DataLayer::Repository::Instance().GetGamePlayDataSource()->IsTENowTheme(mActiveGame.ThemeId))
            {
                if (!mUnloadedPrimaryGameForSitAndGoTransition)
                {
                    // a "standalone" SitAndGo theme has been unloaded, indicating the end of this SitAndGo tournament transition arc
                    mIsInSitAndGoTransitionArc = false;
                }
            }
            else
            {
                // flag that a primary, non-SitAndGo theme was unloaded as part of this SitAndGo tournament transition arc
                mUnloadedPrimaryGameForSitAndGoTransition = true;
            }
        }

        CustomizedOverlayManager::getSingletonPtr()->releaseCapture();

        const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
        std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PluginOnGameUnload), this));

        OgreResources::Instance()->PurgeResources(mActiveGame.ThemeId);
        CustomizedOverlayManager::getSingletonPtr()->destroyAllOverlayElements(false);
        CustomizedOverlayManager::getSingletonPtr()->destroyAllOverlayElements(true);

        // Unload any bolt-on-bonuses.
        if (!mActiveGame.NitroBonusData.empty())
        {
            for (const auto& curBonusData : mActiveGame.NitroBonusData)
            {
                const std::string& packagePath = curBonusData.get("path", "").asString();
                poco_assert(!packagePath.empty());

                Json::Value unloadMsg;
                Message::InitializeMessage(GROUP_SYSTEM, "SuggestUnloadPackage", Message::EVENT, unloadMsg);
                unloadMsg[Json::StaticString("pluginRequestType")] = "DLCPackageRequest_GameEngine";
                unloadMsg[Json::StaticString("packagePath")] = packagePath;
                SendEvent(unloadMsg);
            }

            mpRenderQueueController.reset();
            mNitroBonusThemes.clear();
        }

        // background loading:
        // stop game may have been sent before textures were prepared, so the idle scene may not have been unloaded.
        // TODO: split idlescene load into a 'load' and a 'start/display'
        mpIdleScene->Unload();

        logLoadedResources();

        //Turn off stream override
        if (!mActiveBundle.empty() || DataLayer::Repository::Instance().GetGamePlayDataSource()->GetIsLanguageToggleForTheme(mActiveGame.ThemeId))
        {
            auto bink_plugin = (BinkWidgetPlugin*)getPluginByName("Plugin_BinkWidget");
            if (bink_plugin)
            {
                bink_plugin->OverrideStreamSetting(false);
            }
        }

        if (loadIdle)
        {
            mpIdleScene->Load();
            if (mShowSitAndGoLoadingScene)
            {
                mpIdleScene->ShowLoadingOverlays(false);
                mpIdleScene->ShowLoadingScene(true);
            }
            else
            {
                if (!mActiveBundle.empty())
                {
                    mpIdleScene->ShowLoadingOverlays(false);
                }
                if (mbSwitchLanguage)
                {
                    mpIdleScene->SetActiveThemeId(mActiveGame.ThemeId);
                    mpIdleScene->ShowLoadingOverlays(false);
                    mpIdleScene->ShowLoadingScene(true);
                    mpIdleScene->ForceUpdateDrawLoading();
                    mbSwitchLanguage = false;
                }
                else
                {
                    mpIdleScene->ShowLoadingScene(false);
                }
                mLoadMessage = "";
                mpIdleScene->SetMessage("");
            }
        }

        if (mActiveBundle.empty())
        {
            mpMasterUI->EnableDemoMenuButton(false);
        }

        DeactivateDemoMenu(true);
        mpMasterUI->DeactivateTechScreen();

        AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
        if (pAtlasPlugin)
        {
            pAtlasPlugin->getTextureAtlas("Game")->shutDown();
        }
        std::string stitchingType = DataLayer::Repository::Instance().GetGamePlayDataSource()->GetStitchingType(mActiveGame.ThemeId);
        removeStitchingViewports(stitchingType);

        if (mIsTENowActive)
        {
            unloadTransientPlugin("Plugin_TournEventNow");
            mIsTENowActive = false;
        }

        // Restore the window resolutions in case the game has changed it
        RestoreWindowResolutions();
    }

    setTargetFPS(mDefaultFPS);
}

void OgreApplication::readyToStart()
{
    if(!mIsGameEngineTypeUnity)
    {
        // change the load status so the main thread can start loading textures to the card. 
        mLoadStatus = LOAD_TEXTURE;
        setTargetFPS(15);
        mTextureTimer.reset();
    }
}

void OgreApplication::setTexturesPrepared()
{
    mLoadStatus = LOAD_TEXTUREPREPARED;
}

// this is really when an OGRE game is ready to start.
// when the script calls readyToStart, we still need to create the textures for the game.
void OgreApplication::texturesPrepared()
{
    mpOgreLog->logMessage(String("Textures downloaded in " + Poco::NumberFormatter::format(mTextureTimer.getMilliseconds()) + "ms"));

    // The primary theme is loaded.
    mLoadDependencyMgr.setDependencyLoaded(mActiveGame.ThemeId, true);
    mLoadStatus = LOAD_PRIMARY_COMPLETE;

    // Now load any configured nitro bonuses.
    while (!mPendingNitroBonuses.empty())
    {
        const std::string path = mPendingNitroBonuses.front();

        Json::Value loadMsg;
        Message::InitializeMessage(GROUP_SYSTEM, "SuggestLoadPackage", Message::EVENT, loadMsg);
        loadMsg[Json::StaticString("pluginRequestType")] = "DLCPackageRequest_GameEngine";
        loadMsg[Json::StaticString("packagePath")] = path;
        SendEvent(loadMsg);

        mPendingNitroBonuses.pop();
    }

    // If all dependencies are loaded, allow the system to start the game script(s).
    if (mLoadDependencyMgr.allDependenciesLoaded())
    {

        // If the prior condition was not met, it will be checked again in ExtraResourcesLoaded().
        onLoadComplete();
    }
    else
    {
        // Display a load message indicating the EGM is awaiting bonus package authentication.
        SetNextLoadMessage("Awaiting authentication from host for configured Nitro Bonus package(s)");
    }


}

void OgreApplication::onLoadComplete()
{
    // make sure the next call to process won't skip a frame.
    mLastUpdateTime = 0;
    mLoadStatus = LOAD_COMPLETE;
    mRoot->clearEventTimes();
    setTargetFPS(mDefaultFPS);

    if (!mHibernating)
    {
        bool scriptsLoadFailed = false;

        DataLayer::Attributes attributes;
        bool demoMenuEnabled = false;
        attributes.getConfigValue("gameengine.EnableDemoMenu", demoMenuEnabled);
        mpMasterUI->EnableDemoMenuButton(demoMenuEnabled);

        // force all of the overlays to initialize
        Ogre::OverlayManager::OverlayMapIterator elementIt = Ogre::OverlayManager::getSingleton().getOverlayIterator();
        while (elementIt.hasMoreElements())
        {
            Overlay* pOverlay = elementIt.getNext();
            pOverlay->show();
            pOverlay->hide();
        }

        try
        {
            const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
			std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PluginOnActiveThemeIdUpdate), this));
            std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PluginOnGameLoad), this));
            std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PluginOnSetLanguage), this));
        }
        catch (Poco::Exception& e)
        {
            scriptsLoadFailed = true;
            mpOgreLog->logMessage("Script Load Failure", LML_CRITICAL);
            mpOgreLog->logMessage(e.message().c_str(), LML_CRITICAL);
        }

        CustomizedOverlayManager::getSingletonPtr()->enableOverlaySorting(true);
        CustomizedOverlayManager::getSingletonPtr()->updateOverlayOrder();

        // Only send out "GameDataLoadedNotification" when we have mActiveGame.
        // If comes here by loading complete of MGUI, do not send this message.
        if (!mActiveGame.ThemeName.empty())
        {
            Ogre::Real horizontalScaleFactor=.0f, verticalScaleFactor=.0f;
            int verticalDRRAdjustment = 0;

            // let GP Menu plugin know if BP was rescaled
            Json::Value rescaleNotification;
            MFORCE::Messaging::Message::InitializeMessage(GROUP_GPMENUCONTROL, MFORCE::GP::GPCtrl, MFORCE::Messaging::Message::EVENT, rescaleNotification);
            rescaleNotification[MFORCE::GP::GPField_Cmd] = MFORCE::GP::GPEvent_ButtonPanelScaling;
            rescaleNotification[MFORCE::GP::GPField_ButtonPanelScaled] = false;

            if (ShouldScaleButtonPanel(horizontalScaleFactor, verticalScaleFactor, verticalDRRAdjustment))
            {
                ScaleButtonPanelAndPositionCamera(horizontalScaleFactor, verticalScaleFactor);
                mIsButtonPanelRescaled = true;
                mDRRRescaleAdjustment = verticalDRRAdjustment;
                rescaleNotification[MFORCE::GP::GPField_ButtonPanelScaled] = true;
            }

            OnGPMenuCtrlMsg(rescaleNotification);

            // notify internal entities
            NotificationManager::Instance().PostNotification(new MFORCE::GamePlay::GameLoadedNotification(mActiveGame));
            // notify the rest of the system
            Json::Value dataLoaded;
            Message::InitializeMessage(GROUP_SYSTEM, "GameDataLoadedNotification", Message::EVENT, dataLoaded);
            dataLoaded["themeName"] = mActiveGame.ThemeName;
            dataLoaded["gamePlayId"] = mActiveGame.Id;
            dataLoaded["moduleName"] = MODULENAME_GAMEENGINE;
            dataLoaded["success"] = !scriptsLoadFailed;
            SendEvent(dataLoaded);
            mpOgreLog->logMessage(String("Game Loaded: " + mActiveGame.ThemeName));

            if (mIsInSitAndGoTransitionArc && !DataLayer::Repository::Instance().GetGamePlayDataSource()->IsTENowTheme(mActiveGame.ThemeId))
            {
                // completed load of a non-SitAndGo theme indicates that we're no longer in the SitAndGo tournament transition arc
                mIsInSitAndGoTransitionArc = false;
                mUnloadedPrimaryGameForSitAndGoTransition = false;
            }

            poco_notice(Poco::Logger::get("gameengine"), Poco::format("Game %s loaded and ready to start.", mActiveGame.ThemeName));
        }
    }
}

void MFORCE::GameEngine::OgreApplication::onSetupDisplayedEvent(const Json::Value&)
{
    // this function is triggered by a game after it is done processing DisplaySetup event
    // now that the game is done with DisplaySetup event, check every window for any game displayed overlays, if no overlays are shown display the default logo. 
    DisplayLogoIfNoOtherOverlaysArePresent();

    mRecheckWindowsForDefaultLogo = true; // button panel overlay is overriden and displayed by a game as a result of PlayBarChanged message. 
                                          // if this flag is set button panel window will be checked and cleared of the logo after the PlayBarDataChanged message is processed by the game
                                          // setting this flag will make PlayBarDataChanged event to recheck all windows for logo presence and clear it
}

void MFORCE::GameEngine::OgreApplication::DisplayLogoIfNoOtherOverlaysArePresent()
{
    auto overlayManagerPtr = CustomizedOverlayManager::getSingletonPtr();

    for (auto& item : mViewports)
    {
        unsigned shownOverlays = 0;
        std::string name = item.first;

        //VTOP1 and VTOP2 show the spinning "E" logo by default
#if DEVELOPER
        if (strstr(name.c_str(), MemoryRenderBuffer::GetPostfix().c_str()) || name == "MAIN" || name == "VTOP1" || name == "VTOP2")
#else
        if (name == "MAIN" || name == "VTOP1" || name == "VTOP2")
#endif
        {
            continue;
        }

        Ogre::Overlay* defaultOverlay = Ogre::OverlayManager::getSingleton().getByName("Core/DefaultScreenOverlay");
        if (!defaultOverlay)
        {
            poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Failed to find Core/DefaultScreenOverlay");
            return;
        }
        auto logoElement = name == "MINI" ?
            static_cast<SpriteOverlayElement*>(overlayManagerPtr->getOverlayElement("Core/DefaultScreen.LogoHorz")) :
            static_cast<SpriteOverlayElement*>(overlayManagerPtr->getOverlayElement("Core/DefaultScreen.Logo"));

        if (!logoElement)
        {
            poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Failed to find Core/DefaultScreen.Logo");
            return;
        }
        auto bgElement = static_cast<SpriteOverlayElement*>(overlayManagerPtr->getOverlayElement("Core/DefaultScreen.BackgroundImage"));
        if (!bgElement)
        {
            poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Failed to find Core/DefaultScreen.BackgroundImage");
            return;
        }

        Ogre::Viewport* viewport = item.second;
        auto overlays = CustomizedOverlayManager::getSingleton().getViewportOverlays(viewport);


        for (auto& overlayStr : overlays)
        {
            auto overlayPtr = overlayManagerPtr->getByName(overlayStr);
            bool visible = overlayManagerPtr->isOverlayVisibleForViewport(viewport, overlayPtr);
            if (visible)
            {
                ++shownOverlays;
            }
            poco_trace(Poco::Logger::get(MODULENAME_GAMEENGINE), "Viewport: " + name + " Overlay: " + overlayStr + " visible: " + (visible ? "true" : "false"));
        }

        if (shownOverlays == 0)
        {
            poco_trace(Poco::Logger::get(MODULENAME_GAMEENGINE), "Viewport: " + name + " is blank, use Default Screen Overlay");

            PrepDefaultScreenForViewport(name, viewport, bgElement, logoElement);

            overlayManagerPtr->showOverlay(viewport, defaultOverlay, true);
        }
        else
        {
            overlayManagerPtr->hideOverlay(viewport, defaultOverlay);
        }
    }
}

void MFORCE::GameEngine::OgreApplication::PrepDefaultScreenForViewport(const std::string& name, const Ogre::Viewport* viewport, MFORCE::GameEngine::SpriteOverlayElement* bgElement, MFORCE::GameEngine::SpriteOverlayElement* logoElement)
{
    int vWidth = viewport->getActualWidth();
    int vHeight = viewport->getActualHeight();
    float lWidth = logoElement->getWidth();
    float lHeight = logoElement->getHeight();
    logoElement->show();

    if ("MINI" == name)
    {
        //MINI viewport size is set to 640X480 while the physical monitor displays 640X240 so here we prepare the assets to support the actual display size
        logoElement->setLeft((vWidth / 2.0f - lWidth / 2.0f));
        logoElement->setTop((vHeight / 4.0f - lHeight / 2.0f));
    }
    else
    {
        logoElement->setLeft((vWidth / 2.0f - lWidth / 2.0f));
        logoElement->setTop((vHeight / 2.0f - lHeight / 2.0f));
    }

}

void OgreApplication::eventOccurred(const String& eventName, const NameValuePairList* parameters)
{
#if WIN32
    if (eventName.compare("DeviceLost") == 0)
    {
    }
    else if (eventName.compare("DeviceRestored") == 0)
    {
    }
#endif
}

void OgreApplication::onGameInPlay(const Poco::AutoPtr<GameInPlayNotification>& notification)
{
    setGameInPlay(notification->IsGameInPlay);
}

void OgreApplication::onEffectsProfileEvent(const Json::Value& msg)
{
    if (mpMasterUI != NULL)
    {
        mpMasterUI->OnEffectsProfileEvent(msg);
    }
}

void OgreApplication::onEqualizerEvent(const Json::Value& msg)
{
    std::string msgFunction = msg.get("msgFunction", "").asString();
    if (msgFunction == "ExitEqualizerDisplay")
    {
        if ((mpMasterUI != NULL) && (mpMasterUI->IsEqualizerUIActive()))
        {
            mpMasterUI->DeactivateEqualizerUI();
        }
    }
}

void OgreApplication::inputEvent(IOEvents::InputEventID aEventID, bool aIsVirtual, int aHWInputID, bool aSwitchState)
{
    InputEventBroadcaster::instance().processInputEvent(aEventID, aIsVirtual, aHWInputID, aSwitchState);

    switch (aEventID)
    {
        case IOEvents::I_DEMOSCREEN:
        {
            OgreApplication& app = OgreApplication::getSingleton();

            Json::Value playCueCommand;
            MFORCE::Messaging::Message::InitializeMessage(GROUP_AUDIO, "PlayCueCommand", MFORCE::Messaging::Message::EVENT, playCueCommand);
            playCueCommand["cueName"] = "DemoButton";
            app.SendEvent(playCueCommand);

            Json::Value getDemoMenu;
            MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM, "GetDemoMenuEvent", MFORCE::Messaging::Message::EVENT, getDemoMenu);
            app.SendEvent(getDemoMenu);
        }
        break;
        case IOEvents::I_UNITY_DEMOSCREEN:
        {
            OgreApplication& app = OgreApplication::getSingleton();

            bool isDemoMenuActive = app.GetDemoMenuUI()->IsActive();
            if (isDemoMenuActive)
            {
                app.DeactivateDemoMenu();
            }
            else
            {
                Json::Value value;
                app.ActivateDemoMenu("", value, -1, true);
            }
        }
        break;
        case IOEvents::I_SWITCHGAMES:
        {
            // If switching games in a multi-game environment, make sure to clean up the demo menu.
            OgreApplication& app = OgreApplication::getSingleton();
            app.DeactivateDemoMenu();
        }
        break;
        case IOEvents::I_GPM_SERVICE:
        case IOEvents::I_CALLATTENDANT:
        {
            // if confirmation UI is supported, platform would always get I_GPM_SERVICE event when service button is pressed
            // Platform would also get I_CALLATENDANT event if service confirmation ui is disabled or player presses the yes button
            // ignore I_CALLATTENDANT event if service confirmation ui is supported
            // if confirmation UI is not supported, platform would only get I_CALLATTENDANT event
            bool ignoreEvent = MFORCE::GPMenu::GetDynastyFamilyButtonsSettings().IsSupported() && (aEventID == IOEvents::I_CALLATTENDANT);
            if (!ignoreEvent)
            {
                Json::Value msg;
                MFORCE::Messaging::Message::InitializeMessage(GROUP_GPMENUCONTROL, "GameNotification_ServiceButtonPressed", MFORCE::Messaging::Message::EVENT, msg);
                GameEngineMessageBroadcast::instance().broadCastMessage("GameNotifications", msg);

                DisplayArgs displayArgs("0", "{}");
                NotificationManager::Instance().PostNotification(Poco::Notification::Ptr(new ServiceButtonPressNotification(displayArgs)));
            }
        }
        break;
        case IOEvents::I_GPM_CASHOUT:
        case IOEvents::I_CASHOUT:
        {
            // we might get both signals if confirmation ui is supported
            // ignore I_CASHOUT event if confirmation ui is supported
            bool ignoreEvent = MFORCE::GPMenu::GetDynastyFamilyButtonsSettings().IsSupported() && (aEventID == IOEvents::I_CASHOUT);
            if (!ignoreEvent)
            {
                Json::Value msg;
                MFORCE::Messaging::Message::InitializeMessage(GROUP_GPMENUCONTROL, "GameNotification_CashoutButtonPressed", MFORCE::Messaging::Message::EVENT, msg);
                GameEngineMessageBroadcast::instance().broadCastMessage("GameNotifications", msg);

                DisplayArgs displayArgs("0", "{}");
                NotificationManager::Instance().PostNotification(Poco::Notification::Ptr(new CashoutButtonPressNotification(displayArgs)));
            }
        }
        break;
    default:
        // Do nothing if input event not known
        break;
    }
}

bool OgreApplication::shouldRejectTouch(const OIS::MouseEvent& aEvent)
{
    //If we're within a pixel of an edge, we're going to ignore it
    if (aEvent.state.X.abs < 5 || aEvent.state.X.abs > aEvent.state.width - 5 ||
        aEvent.state.Y.abs < 5 || aEvent.state.Y.abs > aEvent.state.height - 5)
    {
        poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), "Rejected Touch");
        return true;
    }
    return false;
}

void OgreApplication::injectMouseButtonDown(Ogre::RenderWindow* pWindow, const OIS::MouseEvent& aEvent, OIS::MouseButtonID aButtonId)
{
    poco_trace(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Mouse Down: Window: %s X: %d Y: %d", pWindow->getName(), aEvent.state.X.abs, aEvent.state.Y.abs));

    if (shouldRejectTouch(aEvent))
        return;

    mMouseDispatcher.dispatchMouseButtonDown(pWindow, aEvent, aButtonId);
    if (mpScriptEngine != NULL)
    {
        double x = (double)aEvent.state.X.abs / (double)aEvent.state.width;
        double y = (double)aEvent.state.Y.abs / (double)aEvent.state.height;

        mpScriptEngine->addMouseEvent(ScriptEngine::kPressed, x, y, (int)aButtonId, pWindow->getName());
    }
}

void OgreApplication::injectMouseButtonUp(Ogre::RenderWindow* pWindow, const OIS::MouseEvent& aEvent, OIS::MouseButtonID aButtonId)
{
    poco_trace(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Mouse Up: Window: %s X: %d Y: %d", pWindow->getName(), aEvent.state.X.abs, aEvent.state.Y.abs));
    mMouseDispatcher.dispatchMouseButtonUp(pWindow, aEvent, aButtonId);
    if (mpScriptEngine != NULL)
    {
        double x = (double)aEvent.state.X.abs / (double)aEvent.state.width;
        double y = (double)aEvent.state.Y.abs / (double)aEvent.state.height;

        mpScriptEngine->addMouseEvent(ScriptEngine::kReleased, x, y, (int)aButtonId, pWindow->getName());
    }
}

void OgreApplication::injectMouseMove(Ogre::RenderWindow* pWindow, const OIS::MouseEvent& aEvent)
{
    static int previousX = 0;
    static int previousY = 0;

    mMouseDispatcher.dispatchMouseMove(pWindow, aEvent);
    if (mpScriptEngine)
    {
        if (aEvent.state.X.abs != previousX || aEvent.state.Y.abs != previousY)
        {
            double x = (double)aEvent.state.X.abs / (double)aEvent.state.width;
            double y = (double)aEvent.state.Y.abs / (double)aEvent.state.height;

            mpScriptEngine->addMouseEvent(ScriptEngine::kMoved, x, y, -1, pWindow->getName());
        }

        previousX = aEvent.state.X.abs;
        previousY = aEvent.state.Y.abs;
    }
}

void OgreApplication::injectKeyDown(const OIS::KeyEvent& aEvent, unsigned int aModifiers)
{
    mKeyboardDispatcher.dispatchKeyDown(aEvent, aModifiers);

    if (mpHardware != NULL)
    {
        mpHardware->onKeyPress(aEvent);
    }
    mKeyboardShortcuts.onKey(aEvent.key, aModifiers);
}

void OgreApplication::injectKeyUp(const OIS::KeyEvent& aEvent, unsigned int aModifiers)
{
    mKeyboardDispatcher.dispatchKeyUp(aEvent, aModifiers);

    if (mpHardware != NULL)
    {
        mpHardware->onKeyRelease(aEvent);
    }
}

void OgreApplication::ReloadAssets()
{
    if (mpMasterUI)
    {
        mpMasterUI->DisplayUnavailableScreens(true);

        if (mpUserMessageController)
            mpUserMessageController->setEGMMessage("Reloading...");
        mRoot->renderOneFrame(1.0f);
        std::set<std::string> updated;
    #if defined(DEVELOPER)
        try
    #endif
        {
            OgreResources::Instance()->GetResourceCatalog().RefreshModificationTimes(updated);
        }
    #if defined(DEVELOPER)
        catch (Poco::Exception& ex)
        {
            throw MFORCE::Foundation::ScriptException("ReloadAssets", mActiveGame.ThemeName, "", ex.displayText());
        }
    #endif

        if (updated.empty() == false)
        {
            bool javascriptUpdated = false;
            bool audioUpdated = false;
            std::set<std::string> imagesUpdated;

            std::set<std::string>::iterator iter = updated.begin();
            while (iter != updated.end())
            {
                // cheesy check based on file extension
                Poco::Path p(*iter);
                std::string lowerExt = Poco::toLower(p.getExtension());
                if (lowerExt.compare("js") == 0)
                {
                    updated.erase(iter);
                    iter = updated.begin();
                    javascriptUpdated = true;
                }
                else if ((lowerExt.compare("wav") == 0) || (lowerExt.compare("xap") == 0))
                {
                    updated.erase(iter);
                    iter = updated.begin();
                    audioUpdated = true;
                }
                else if ((lowerExt.compare("png") == 0)
                    || (lowerExt.compare("dds") == 0)
                    || (lowerExt.compare("bmp") == 0)
                    )
                {
                    imagesUpdated.insert(*iter);
                    updated.erase(iter);
                    iter = updated.begin();
                }

                else
                    ++iter;
            }

            // anything else? (binks, fonts, particles, ...) reload the game completely.
            if (updated.empty() == false)
            {
                // during reload, reloaded items get removed from 'updated'.
                // if anything is still here, trigger a game reload, as we weren't able to update it
                Json::Value reloadGame;
                Message::InitializeMessage(GROUP_SYSTEM, "ReloadCurrentGame", Message::EVENT, reloadGame);
                SendEvent(reloadGame);
            }
            else
            {
                if (javascriptUpdated)
                {
                    // open/close a door to reload the js and do recovery
                    virtualButtonEvent(std::string("VB_MAINDOOR_OPENED"), false);
                    virtualButtonEvent(std::string("VB_MAINDOOR_CLOSED"), false);
                }
                if (audioUpdated)
                {
                    SendReloadAudioEvent();
                }
                if (imagesUpdated.empty() == false)
                {
                    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
                    if (pAtlasPlugin)
                        pAtlasPlugin->getTextureAtlas("Game")->reload(imagesUpdated);
                }
            }
        }

        RenderWindow* pMainWindow = getPrimaryRenderWindow();
        pMainWindow->resetStatistics();
        mpMasterUI->DisplayUnavailableScreens(false);
        if (mpUserMessageController)
            mpUserMessageController->setEGMMessage("");

        mLastEGMMessage = "";
    }
}

void OgreApplication::virtualButtonEvent(const std::string& aButtonId, bool aPressed)
{
    // send the event out over the bus
    Json::Value virtualButton;
    Message::InitializeMessage(GROUP_IOINPUT, "VirtualButtonEvent", Message::EVENT, virtualButton);
    virtualButton["buttonId"] = aButtonId;
    virtualButton["pressed"] = aPressed;

    OgreApplication::getSingleton().SendEvent(virtualButton);

    if (mpCautionScene)
    {
        if (aButtonId == "VB_MAINDOOR_OPENED")
        {
            mpCautionScene->ShowCautionOverlays();
        }
        else if (aButtonId == "VB_MAINDOOR_CLOSED")
        {
            mpCautionScene->HideCautionOverlays();
        }
    }
}

void OgreApplication::SendEmulatedBinaryInputEvent(int binaryInputID, bool state)
{
    // send the event out over the bus
    Json::Value binaryInputEvent;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_EMULATION_IOINPUT, "BinaryInputEvent", MFORCE::Messaging::Message::EVENT, binaryInputEvent);
    binaryInputEvent["inputId"] = binaryInputID;
    binaryInputEvent["value"] = state;

    OgreApplication::getSingleton().SendEvent(binaryInputEvent);
}

void OgreApplication::messageLogged(const String& message, LogMessageLevel lml, bool maskDebug, const String &logName)
{
    switch (lml)
    {
    case LML_TRIVIAL:
        poco_trace(Poco::Logger::get(MODULENAME_GAMEENGINE), message);
        break;

    case LML_NORMAL:
        poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), message);
        break;

    case LML_CRITICAL:
        // Change to warning from error since most/all are warnings instead of errors
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), message);
        break;
    }
}

void OgreApplication::ShowDemoMenu(const Json::Value & msg)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    const Json::Value themesData = msg.get("themes", Json::nullValue);
    poco_assert(!themesData.isNull());
    const Json::Value activeThemeData = themesData.get(mActiveGame.ThemeId, Json::nullValue);
    poco_assert(!activeThemeData.isNull());
    bool haveDemoMenu = activeThemeData.get("HaveDemoMenu", true).asBool();

    bool isPrizeEvaluationMode = msg.get("evaluationMode", false).asBool();

    if (haveDemoMenu || isPrizeEvaluationMode)
    {
        if (mpDemoMenuUI == NULL)
        {
            mpDemoMenuUI = new DemoMenuUI();
        }
        if (IsDemoMenuActive())
        {
            DeactivateDemoMenu();
        }
        else
        {
            ActivateDemoMenu(mActiveGame.ThemeId, msg, mReelIndex);
        }
    }
    else
    {
        // Originator of the show demo menu event may be waiting for completion notification.
        SendDemoMenuComplete("", false);
    }
}

void OgreApplication::SendDemoMenuComplete(std::string cmd, bool gaffed)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), "SendDemoMenuComplete");

    Json::Value msg;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_GAMEENGINE, "DemoMenuComplete", MFORCE::Messaging::Message::EVENT, msg);
    msg["command"] = cmd;
    msg["gaffed"] = gaffed;
    SendEvent(msg);
}

void OgreApplication::OnPrizeFirstData(const Json::Value & msg)
{
    if (IsDemoMenuActive())
    {
        mpDemoMenuUI->OnPrizeFirstData(msg);
    }

    processPatsMessages(msg);
}

void OgreApplication::OnDemoMenuData(const Json::Value & msg)
{
    if (IsDemoMenuActive())
    {
        mpDemoMenuUI->OnDemoMenuData(msg);
    }
}

void OgreApplication::OnDenomChanged()
{
    if (mpDemoMenuUI)
    {
        mpDemoMenuUI->OnDenomChanged();
    }
}

bool OgreApplication::IsMessageBoxConfirmClick(Ogre::RenderWindow* pWindow, const OIS::MouseEvent& aEvent, OIS::MouseButtonID aButtonId)
{
    if (mpMasterUI->IsMessageBoxActive())
        return mpMasterUI->IsMessageBoxConfirmClick(pWindow, aEvent, aButtonId);

    return false;
}

bool OgreApplication::IsMessageBoxRejectClick(Ogre::RenderWindow* pWindow, const OIS::MouseEvent& aEvent, OIS::MouseButtonID aButtonId)
{
    if ((NULL != mpMasterUI) && mpMasterUI->IsMessageBoxActive())
        return mpMasterUI->IsMessageBoxRejectClick(pWindow, aEvent, aButtonId);

    return false;
}

void OgreApplication::ShowMessageBox(const std::string& titleString, const std::string& descriptionString, int msgtype, Poco::UInt64 displayDuration)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    mpMasterUI->ShowMessageBox(titleString, descriptionString, msgtype, displayDuration);

    Json::Value PlayCueCommand;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_AUDIO, "PlayCueCommand", MFORCE::Messaging::Message::EVENT, PlayCueCommand);
    PlayCueCommand["cueName"] = "MsgBoxIn";
    SendEvent(PlayCueCommand);
}

void OgreApplication::ClearMessageBox()
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    if ((NULL != mpMasterUI) && mpMasterUI->IsMessageBoxActive())
    {
        mpMasterUI->HideMessageBox();

        Json::Value PlayCueCommand;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_AUDIO, "PlayCueCommand", MFORCE::Messaging::Message::EVENT, PlayCueCommand);
        PlayCueCommand["cueName"] = "MsgBoxOut";
        SendEvent(PlayCueCommand);
    }
}

void OgreApplication::SetCurrentVideoReelLayout(const std::vector<BaseVideoReel*> & newVideoReel)
{
    mCurrentReelLayout.clear();

    mCurrentReelLayout = newVideoReel;
}

void OgreApplication::logGraphicsDeviceResourceInfo()
{
#if WIN32
#if DEVELOPER
    // NOTE: only meaningful when using the d3d debug runtime

    // prevent failure on window reinitialization by ignoring until reinitialization known to be done
    if (mLoadStatus != LOAD_COMPLETE)
    {
        return;
    }

    std::string outputMsg("Graphics Device Resource Info:\n");

    for (WindowMap::iterator it = mWindows.begin(); it != mWindows.end(); ++it)
    {
        outputMsg.append(Poco::format("Device: %s\n", it->first));

        IDirect3DDevice9* d3ddevice = NULL;
        (*it).second->getCustomAttribute("D3DDEVICE", &d3ddevice);
        if (d3ddevice != NULL)
        {
            IDirect3DQuery9* query;
            HRESULT hr = ((IDirect3DDevice9*)d3ddevice)->CreateQuery(D3DQUERYTYPE_RESOURCEMANAGER, &query);
            if (hr == S_OK)
            {
                hr = query->Issue(D3DISSUE_END);
                if (hr == S_OK)
                {
                    int datasize = query->GetDataSize();

                    void *data = malloc(datasize);
                    if (data != NULL)
                    {
                        hr = query->GetData(data, datasize, D3DGETDATA_FLUSH);
                        if (hr == S_OK)
                        {
                            int i = 0;

                            D3DRESOURCESTATS* s = (D3DRESOURCESTATS*)data;

                            while (datasize > 0)
                            {
                                outputMsg.append(Poco::format("Resource type:          %?d - ", i));

                                static char* resource_type[] =
                                {
                                    "?",
                                    "Surface",
                                    "Volume",
                                    "Texture",
                                    "Volume Texture",
                                    "Cube Texture",
                                    "Vertex Buffer",
                                    "Index Buffer"
                                };

                                char* type = "unknown";
                                if (i < 8)
                                {
                                    type = resource_type[i];
                                }

                                outputMsg.append(type).append("\n");

                                std::string thrashing(s->bThrashing ? "Yes" : "No");
                                outputMsg.append(Poco::format("Thrashing:              %s\n", thrashing));
                                outputMsg.append(Poco::format("ApproxBytesDownloaded:  %?d\n", s->ApproxBytesDownloaded));
                                outputMsg.append(Poco::format("NumEvicts:              %?d\n", s->NumEvicts));
                                outputMsg.append(Poco::format("NumVidCreates:          %?d\n", s->NumVidCreates));
                                outputMsg.append(Poco::format("LastPri:                %?d\n", s->LastPri));
                                outputMsg.append(Poco::format("NumUsed:                %?d\n", s->NumUsed));
                                outputMsg.append(Poco::format("NumUsedInVidMem:        %?d\n", s->NumUsedInVidMem));
                                outputMsg.append(Poco::format("WorkingSet:             %?d\n", s->WorkingSet));
                                outputMsg.append(Poco::format("WorkingSetBytes:        %?d\n", s->WorkingSetBytes));
                                outputMsg.append(Poco::format("TotalManaged:           %?d\n", s->TotalManaged));
                                outputMsg.append(Poco::format("TotalBytes:             %?d\n", s->TotalBytes));

                                ++i;
                                ++s;
                                datasize -= sizeof(D3DRESOURCESTATS);
                            }

                            outputMsg.append("\n");
                        }

                        free(data);
                    }

                    query->Release();
                }
            }
        }
    }

    LogManager::getSingleton().logMessage(outputMsg, Ogre::LML_CRITICAL);
#endif
#endif
}

void OgreApplication::setTargetFPS(unsigned int fps)
{
    mTargetFPS = fps;
    mFPSInterval = 1000 / mTargetFPS;

    // clear frame avg stats on target fps change
    mLastDeltasToAvgIdx = 0;
    int x = 1000000 / mTargetFPS;
    for (int i = 0; i < FRAMES_TO_AVG; ++i)
    {
        mLastDeltasToAvg[i] = x;
    }

    for (int i = 0; i < FRAMES_TO_TRACK; ++i)
    {
        mLastDeltasToTrack[i] = 0;
    }
    mpOgreLog->logMessage(Poco::format("Target frame rate set to %?d FPS", mTargetFPS));
}

void OgreApplication::setLoadMessage(const std::string& message)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    if (mpIdleScene)
    {
        std::string loadMsg = "Loading - ";
        if (!mActiveGame.ThemeName.empty())
        {
            loadMsg += (LOAD_PRIMARY_COMPLETE == mLoadStatus) ? "Nitro Bonus" : mActiveGame.ThemeName;
            loadMsg += " - ";
        }
        loadMsg += message;
        mpIdleScene->SetMessage(loadMsg);
    }
}

void OgreApplication::setMessage(const std::string& message)
{
    if (mpIdleScene)
        mpIdleScene->SetMessage(message);
}

void OgreApplication::updateElapsedMessage()
{
    if (mpIdleScene)
        mpIdleScene->SetElapsedMessage(Poco::NumberFormatter::format(mLoadTimer.getMilliseconds() / 1000.0f, 2));
}

void OgreApplication::setElapsedMessage(const std::string& message)
{
    if (mpIdleScene)
    {
        mpIdleScene->SetElapsedMessage(message);
        mRoot->renderOneFrame(0);
    }
}

void OgreApplication::setLoadError(const std::string& err)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);
    mLoadError = err;
}

void OgreApplication::resetLoadTimer()
{
    if (mpIdleScene)
    {
        mpIdleScene->SetElapsedMessage("");
        mLoadTimer.reset();
    }
}

void OgreApplication::updateIdleSceneLoading()
{
    if (mpIdleScene)
    {
        mpIdleScene->UpdateDrawLoading();
    }
}

// \brief Thread safe set of next load message value
//
void OgreApplication::SetNextLoadMessage(const std::string& message)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);
    mLoadMessage = message;
}

// \brief Thread safe get of next load message value
//
const std::string OgreApplication::GetNextLoadMessage()
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);
    return mLoadMessage;
}

#if OGREAPP_EXE
int main()
{
    OgreApplication app(NULL);
    app.Startup();
    //while (app.Process());
    app.Process();

    app.Shutdown();
    return 0;
}
#endif

const std::string& OgreApplication::GetActiveThemeId() const
{
    return mActiveGame.ThemeId;
}

const std::string& OgreApplication::GetActiveThemeName() const
{
    return mActiveGame.ThemeName;
}

void OgreApplication::GetActiveBonusThemeIds(std::vector<std::string>& out_themeIds) const
{
    out_themeIds.clear();
    for (const auto& curBonusData : mActiveGame.NitroBonusData)
    {
        const std::string& themeName = curBonusData.get("themeId", "").asString();
        if (!themeName.empty())
        {
            out_themeIds.push_back(themeName);
        }
    }
}

std::vector<std::string> OgreApplication::GetActiveBonusThemeNames() const
{
    std::vector<std::string> themeNameList;
    for (const auto& curBonusData : mActiveGame.NitroBonusData)
    {
        std::string themeName = curBonusData.get("themeName", "").asString();
        if (!themeName.empty())
        {
            themeNameList.push_back(themeName);
        }
    }

    return themeNameList;
}

std::vector<std::string> OgreApplication::GetActiveBonusThemeIds() const
{
    std::vector<std::string> themeIds;
    for (const auto& curBonusData : mActiveGame.NitroBonusData)
    {
        std::string themeName = curBonusData.get("themeId", "").asString();
        if (!themeName.empty())
        {
            themeIds.push_back(themeName);
        }
    }
    return themeIds;
}

const std::string& OgreApplication::GetActiveThemeVersion() const
{
    return mActiveGame.ThemeVersion;
}

static void sGenerateNewRandomSeed(Poco::Random &pocoRandom,  const Json::Value &playResultJSON)
{
    unsigned int playSequence = playResultJSON["PlaySequenceNumber"].asUInt();
    unsigned int baseAward = playResultJSON["BaseGameAward"].asUInt();
    unsigned int newSeed = playSequence + baseAward + 1;

    // reseed the pocoRandom
    pocoRandom.seed(newSeed);
}

void OgreApplication::onGameplayGameMessage(const Json::Value& aMsg)
{
    // .
    // First do whatever processing that the OgreApp needs to do
    // .

    // pull out the message function
    const std::string& msgFunction = aMsg.get("msgFunction", "").asString();

    // pull out the theme ID.
    const std::string& themeId = aMsg.get("themeId", "").asString();

    // pull out and update the user message
    const std::string& userMessage = aMsg.get("userMessage", "").asString();
    if (mpUserMessageController != NULL)
    {
        mpUserMessageController->setGameplayMessage(userMessage);
    }
    mLastGamePlayMessage = userMessage;

    // pull out the coin in and coin out
    mCoinInMillicents = Poco::NumberParser::parseUnsigned64(aMsg.get("coinIn", "0").asString());
    mCoinOutMillicents = Poco::NumberParser::parseUnsigned64(aMsg.get("coinOut", "0").asString());

    // setup denom sprites if this is the denom event
    std::string denomOverlay;
    Json::Value denomArray;

    const std::string& denoms = aMsg.get("denoms", "").asString();
    Poco::StringTokenizer tokenizer(denoms, ",", Poco::StringTokenizer::TOK_IGNORE_EMPTY | Poco::StringTokenizer::TOK_TRIM);
    Poco::StringTokenizer::Iterator denomIter = tokenizer.begin();
    Poco::StringTokenizer::Iterator denomEnd = tokenizer.end();

    for (; denomIter != denomEnd; ++denomIter)
    {
        denomArray.append(*denomIter);
    }

    denomOverlay = std::string("BaseGame/MultiDenom") + Poco::NumberFormatter::format(tokenizer.count());

    const Json::Value& msgBoxData = aMsg["msgBoxData"];

    if (msgFunction.compare("StartGameScriptEvent") == 0)
    {
        StartGameScript();
        mpMasterUI->DisplayUnavailableScreens(false);
        evictManagedResources();
    }
    else if (msgFunction.compare("StopGameScriptEvent") == 0)
    {
        // Although StopGameScript() also disables the frame rate tracking,
        // we want to avoid hitches during the call to DisplayUnavailableScreens.
        TrackFrameRates(false);

        StopGameScript();

        mpMasterUI->DisplayUnavailableScreens(true);
    }
    else if (msgFunction.compare("DenomSelectionEnterEvent") == 0)
    {
        std::vector<std::string> denomList;
        denomIter = tokenizer.begin();

        for (; denomIter != denomEnd; ++denomIter)
        {
            denomList.push_back(*denomIter);
        }
        denomOverlay = setupDenomSprites(denoms, denomList);
    }
    else if (msgFunction.compare("MsgBoxDisplayEvent") == 0)
    {
        updateMessageBox(true, msgBoxData);
    }
    else if (msgFunction.compare("MsgBoxClearEvent") == 0)
    {
        updateMessageBox(false, msgBoxData);
    }
    else if (msgFunction.compare("JackpotHandpayDisplay") == 0)
    {
        const std::string& jackpotTitle = aMsg["JackpotHandpayData"]["jackpotTitle"].asString();

        unsigned long long cashableAmt = Poco::NumberParser::parseUnsigned64(aMsg["JackpotHandpayData"]["jackpotAmount"].asString());
        std::string prizeString = Localization::GetCurrencyString(cashableAmt);

        const std::string& jackpotAmountStr = aMsg["JackpotHandpayData"]["jackpotAmountString"].asString();
        const std::string& keyToCreditStr = aMsg["JackpotHandpayData"]["KeyToCreditString"].asString();
        const std::string& attendantNotifyStr = aMsg["JackpotHandpayData"]["AttendantNotifyString"].asString();

        if (mpHandpayController)
        {
            mpHandpayController->fireOnHandpay(
                jackpotTitle.c_str(),
                prizeString.c_str(),
                jackpotAmountStr.c_str(),
                keyToCreditStr.c_str(),
                attendantNotifyStr.c_str()
            );
        }
    }
    else if (msgFunction.compare("JackpotHandpayClear") == 0)
    {
        bool gameSpecific = mpHandpayController ? mpHandpayController->fireOnHandpayClear() : false;
        if (!gameSpecific)
        {
            ClearMessageBox();
        }
    }
    else if (msgFunction.compare("PlaySequenceStartedEvent") == 0
        && mLoadStatus == eLoadStatus::LOAD_COMPLETE)
    {
        setGameInPlay(true);
        mSeedNextResult = true;
        // We start tracking frame rates when the first play starts.
        // Trying to track before that can lead to false positives since the game is
        // busy configuring assets after the "SetupComplete" event was generated.
        TrackFrameRates(true);
    }
    else if (msgFunction.compare("HelpEvent") == 0)
    {
        if (mpMasterUI->IsDemoMenuEnabled())
        {
            mpMasterUI->EnableDemoMenuButton(false);
        }
    }
    else if (msgFunction.compare("HelpExitEvent") == 0)
    {
        DataLayer::Attributes attributes;
        bool demoMenuEnabled = false;
        attributes.getConfigValue("gameengine.EnableDemoMenu", demoMenuEnabled);
        mpMasterUI->EnableDemoMenuButton(demoMenuEnabled);
    }
    else if ((msgFunction.compare("EnterAttractState") == 0) || (msgFunction.compare("ExitAttractState") == 0))
    {
        evictManagedResources();
    }
    else if (msgFunction.compare("TournEventStarting") == 0)
    {
        if (mpMasterUI->IsDemoMenuEnabled())
        {
            // See if the demo button should be disabled for TournEvent.
            DataLayer::ConfigAttribute attr;
            if (attr.load("gameengine.HideDemoButtonOnTourney"))
            {
                bool hide = attr.parseBool();
                if (hide)
                    mpMasterUI->EnableDemoMenuButton(false);
            }
        }
    }

    const Json::Value& playResultJSON = aMsg["playResult"];

    // other flags
    bool contextChangePending = aMsg.get("contextChangePending", false).asBool();
    bool handpay = aMsg.get("handpay", false).asBool();
    bool projectedHandpay = aMsg.get("projectedHandpay", false).asBool();
    bool creditsMeterDisplayChangeAllowed = aMsg.get("creditsMeterDisplayChangeAllowed", false).asBool();

    if (msgFunction == "DisplaySetupEvent")
    {
        mCurrentGameCycleID = playResultJSON["PlaySequenceNumber"].asUInt();	//Update mCurrentGameCycleID for game specific data

        sGenerateNewRandomSeed(mRandom, playResultJSON);

        if (contextChangePending)
        {
            mSeedNextResult = true;
        }

        // make sure the next call to process won't skip a frame.
        mLastUpdateTime = 0;

#if DEVELOPER
        if (mSeeThruMgr)
        {
           mSeeThruMgr->NotifyDisplayReady();
        }
#endif
    }
    else if (msgFunction == "DisplayAwardEvent")
    {
        unsigned int totalBet = aMsg["playbardata"]["total_bet"].asUInt();
        unsigned int maxLines = aMsg["playbardata"]["MaxActivePaylines"].asUInt();

        unsigned int denom = aMsg["playbardata"]["denom"].asUInt();

        // calculate the paid bangup amount
        Poco::Int64 startPaid = Poco::NumberParser::parse64(aMsg["playbardata"]["PaidStart"].asString());
        Poco::Int64 endPaid = Poco::NumberParser::parse64(aMsg["playbardata"]["PaidEnd"].asString());
        Poco::Int64 paidAmount = (endPaid - startPaid) / denom;

        // calculate the cash bangup amount
        Poco::Int64 startCash = Poco::NumberParser::parse64(aMsg["playbardata"]["cash"].asString());
        Poco::Int64 endCash = Poco::NumberParser::parse64(aMsg["playbardata"]["endingcash"].asString());
        Poco::Int64 cashAmount = (endCash - startCash) / denom;

        unsigned long long totalAward = paidAmount;
        if (cashAmount > paidAmount)
        {
            totalAward = cashAmount;
        }

        if (mpBigWinController)
        {
            Poco::Int64 awardAmt = totalAward;

            // Need total award not including any side action award.
            // This is nominally provided by the BaseGameTotalAward field.
            // However, non side action games may not even provide BaseGameTotalAward in the play state.  
            Json::Value jv_BaseGameTotalAward = playResultJSON["BaseGameTotalAward"];
            if (!jv_BaseGameTotalAward.isNull())
            {
                // explicit BaseGameTotalAward provided...
                awardAmt = jv_BaseGameTotalAward.asUInt64();
            }

            mpBigWinController->checkForBigWin(totalBet, maxLines, (unsigned int)awardAmt);
        }
    }

    if (mSeedNextResult && msgFunction == "DisplayResultEvent")
    {
        mSeedNextResult = false;
        mCurrentGameCycleID = playResultJSON["PlaySequenceNumber"].asUInt();	//Update mCurrentGameCycleID for game specific data
        sGenerateNewRandomSeed(mRandom, playResultJSON);
    }

    // Adding a few values to the raw message to support the typescript game engine
    Json::Value& Msg = const_cast<Json::Value&>(aMsg);
    Msg["denomOverlay"] = Json::Value(denomOverlay.c_str());
    Msg["handpay"] = Json::Value(handpay);
    // .
    // Next, broadcast it out to the handlers
    // .
    UnifiedMessageBroadcast::instance().broadCastGameMessage(aMsg);

    bool needsAdditionalFrameUpdate = false;
    if (msgFunction == "PlaybarDataChangedEvent")
    {
        needsAdditionalFrameUpdate = true;
        // make sure to execute following block after above line,broadCastGameMessage, so that the game and other components have 'PlaybarDataChangedEvent' message processed.
        // button panel is created and displayed after the game and buttonpanel component have processed 'PlaybarDataChangedEvent' message
        if (mRecheckWindowsForDefaultLogo)
        {
            //now that button panel overlay has been prepared by the game check if default logo is there and remove it
            DisplayLogoIfNoOtherOverlaysArePresent();
            mRecheckWindowsForDefaultLogo = false;
        }
    }


    // .
    // Finally, prepare and send the raw message itself
    // .

    // pull out the playbar data JSON
    const Json::Value& playbarDataJSON = aMsg["playbardata"];

    // pull out the button modes JSON
    const Json::Value& buttonModesJSON = aMsg["buttonModes"];

    // pull out the cabinetStyle JSON
    const Json::Value& cabinetStyleJSON = aMsg["cabinet_style"];

    const Json::Value& lcdButtonPanelJSON = aMsg["hasLCDButtonPanel"];

    // repackage the data to be sent into the javascript
    // into one json
    Json::ObjectWriter objectWriter;
    objectWriter.write("msgFunction", msgFunction);
    objectWriter.write("themeId", themeId);
    objectWriter.write("cabinetStyle", cabinetStyleJSON);
    objectWriter.write("hasLCDButtonPanel", lcdButtonPanelJSON);
    objectWriter.write("playResult", playResultJSON);
    objectWriter.write("playbarData", playbarDataJSON);
    objectWriter.write("contextChangePending", contextChangePending);
    objectWriter.write("handpay", handpay);
    objectWriter.write("denomOverlay", denomOverlay);
    objectWriter.write("denoms", denomArray);
    objectWriter.write("buttonModes", buttonModesJSON);
    objectWriter.write("creditsMeterDisplayChangeAllowed", creditsMeterDisplayChangeAllowed);

    std::string setupPaid = aMsg.get("setupPaid", "").asString();

    objectWriter.write("setupPaid", setupPaid);

    std::string setupCollect = aMsg.get("setupCollect", "").asString();

    objectWriter.write("setupCollect", setupCollect);

    std::string gamePlayStageID = aMsg.get("gamePlayStageID", "").asString();

    objectWriter.write("gamePlayStageID", gamePlayStageID);
    objectWriter.write("projectedHandpay", projectedHandpay);
    if (aMsg.isMember("displayCreditFlag"))
    {
        bool displayCreditFlag = aMsg.get("displayCreditFlag", true).asBool();
        objectWriter.write("displayCreditFlag", displayCreditFlag);
    }
    if (aMsg.isMember("didWagerOptionsChange"))
    {
        bool didWagerOptionsChangeFlag = aMsg.get("didWagerOptionsChange", true).asBool();
        objectWriter.write("didWagerOptionsChange", didWagerOptionsChangeFlag);
    }

    std::string gamePlayMsg = objectWriter.str();

    // send the message
    {
        v8::Locker locker;
        v8::HandleScope hs;
        v8::Handle<v8::Value> args[] =
        {
            v8::String::New(gamePlayMsg.c_str()),
        };
        DispatchScriptEvent(&Events::mGameplayEventFunc, args, 1, themeId);
    }

    if (needsAdditionalFrameUpdate)
    {
        // After a playbar data changed event, we force the GameEngine to go through at least one loop
        // so that the game has "time" to react to it.
        // This was introduced so that a game would get a frame update between the "PlaybarDataChangedEvent"
        // that cleared the collect meter and the "CreditsAdd" message.
        Process();
    }
}

void OgreApplication::OnSetAutoPlayBanner(bool bBannerOn)
{
    mpMasterUI->OnSetAutoPlayBanner(bBannerOn);
}

void OgreApplication::onEGMGameMessage(const Json::Value& aMsg)
{
    // .
    // First do whatever processing that the OgreApp needs to do
    // .

    // pull out the message function
    const std::string& msgFunction = aMsg.get("msgFunction", "").asString();

    // pull out and update the user message
    const std::string& userMessage = aMsg.get("userMessage", "").asString();
    if (mpUserMessageController != NULL)
    {
        mpUserMessageController->setEGMMessage(userMessage);
    }
    mLastEGMMessage = userMessage;

    if (mpMasterUI)
    {
        if (msgFunction.compare("TechState") == 0)
        {
            if (!mpMasterUI->IsTechScreenActive())
            {
                if (mpMasterUI->IsDemoMenuEnabled() && IsDemoMenuActive())
                {
                    DeactivateDemoMenu();
                }
                if (IsGaffingUIActive())
                {
                    mpMasterUI->DeactivateGaffingUI();
                }
                mpMasterUI->ActivateTechScreen();
                setTargetFPS(mTechUIFPS);
            }
        }
        else if (msgFunction.compare("TechStateClear") == 0)
        {
            if (mpMasterUI->IsTechScreenActive())
            {
                mpMasterUI->DeactivateTechScreen();
                setTargetFPS(mDefaultFPS);
            }
        }
        else if (msgFunction.compare("StopGameScriptEvent") == 0) // EGM_Errors can send this on Critical Memory Error or Non-Recoverable Error
        {
            StopGameScript();

            mpMasterUI->DisplayUnavailableScreens(true);
        }
        else if (msgFunction.compare("DisplayUnavailableScreens") == 0)
        {
            mpMasterUI->DisplayUnavailableScreens(true);
        }

    }

    const Json::Value& msgBoxData = aMsg["msgBoxData"];
    if (msgFunction.compare("MsgBoxDisplayEvent") == 0)
    {
        updateMessageBox(true, msgBoxData);
    }
    else if (msgFunction.compare("MsgBoxClearEvent") == 0)
    {
        updateMessageBox(false, msgBoxData);
    }
    else if (msgFunction.compare("CashoutStarted") == 0)
    {
        //		if(mpCashoutController)
        //		{
        //			mpCashoutController->fireOnCashoutStart();
        //		}
    }
    else if (msgFunction.compare("CashoutEvent") == 0 || msgFunction.compare("FundsTransferFromEGMEvent") == 0)
    {
        Json::Value PlayCueCommand;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_AUDIO, "PlayCueCommand", MFORCE::Messaging::Message::EVENT, PlayCueCommand);
        PlayCueCommand["cueName"] = "Cashout";
        SendEvent(PlayCueCommand);

        //		if(mpCashoutController)
        //		{
        //			mpCashoutController->fireOnCashoutEnd();
        //		}
    }

    // Set Status Indicators
    bool paperout = aMsg.get("paperOut", false).asBool();
    bool paperlow = aMsg.get("paperLow", false).asBool();
    bool printerDisabled = aMsg.get("printerDisabled",false).asBool();
    bool batteryLow = aMsg.get("batteryLow", false).asBool();
    bool doorOpen = aMsg.get("doorOpen", false).asBool();
    bool mainDoorOpen = aMsg.get("mainDoorOpen", false).asBool();
    bool powerReset = aMsg.get("powerReset", false).asBool();
    bool snapshot = aMsg.get("snapshot", false).asBool();
    bool snapshotFail = aMsg.get("snapshotFail", false).asBool();
    int linkDownType = aMsg.get("linkDownType", MFORCE::Cabinet::LinkDownType::None).asInt();
    bool creditsMeterDisplayChangeAllowed = aMsg.get("creditsMeterDisplayChangeAllowed", false).asBool();

    int statusFlag = MFORCE::GameEngine::MasterUI::STATUSINDICATOR_NONE;

    if (paperout && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "PAPER_OUT") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_PAPER_OUT;
    }
    if (paperlow && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "PAPER_LOW") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_PAPER_LOW;
    }
    if (printerDisabled && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "PRINTER_DISABLED") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_PRINTER_DISABLED;
    }
    if (batteryLow && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "BATTERY_LOW") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_BATTERY_LOW;
    }
    if (doorOpen && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "DOOR_OPENED") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_DOOR_OPENED;
    }
    if (powerReset && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "POWER_RESET") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_POWER_RESET;
    }
    if (snapshot && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "SNAPSHOT") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_SNAPSHOT;
    }
    if (snapshotFail && std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "SNAPSHOT_FAIL") == mIndicatorsToFilterOut.end())
    {
        statusFlag |= MFORCE::GameEngine::MasterUI::STATUSINDICATOR_SNAPSHOT_FAIL;
        Sound::playCue("SnapshotFail");
    }
    if (std::find(mIndicatorsToFilterOut.begin(), mIndicatorsToFilterOut.end(), "LINK_DOWN") != mIndicatorsToFilterOut.end())
    {
        linkDownType = MFORCE::Cabinet::LinkDownType::LinkDownType::None;
    }

    mpMasterUI->SetStatusIndicator((MFORCE::GameEngine::MasterUI::StatusIndicator)statusFlag, (MFORCE::Cabinet::LinkDownType::LinkDownType)linkDownType);
    if (mpCautionScene)
    {
        if (mainDoorOpen)
        {
            mpCautionScene->ShowCautionOverlays();
        }
        else
        {
            mpCautionScene->HideCautionOverlays();
        }
    }
    // .
    // Next, broadcast it out to the handlers
    // .

    UnifiedMessageBroadcast::instance().broadCastEgmMessage(aMsg);

    // .
    // Finally, prepare and send the raw message itself
    // .

    // repackage the data to be sent into the javascript
    // into one json
    Json::ObjectWriter objectWriter;

    objectWriter.write("msgFunction", msgFunction);
    objectWriter.write("userMessage", userMessage);
    objectWriter.write("msgBoxData", msgBoxData);
    objectWriter.write("egmCash", aMsg.get("cash", "0").asString());
    objectWriter.write("egmNonCash", aMsg.get("noncash", "0").asString());
    objectWriter.write("egmPromo", aMsg.get("promo", "0").asString());
    objectWriter.write("egmCollect", aMsg.get("collect", "0").asString());
    objectWriter.write("egmCreditsIn", aMsg.get("credits_in", "0").asString());
    objectWriter.write("egmDenom", (Json::LargestInt)(aMsg.get("denom", 1000).asInt()));
    objectWriter.write("creditsMeterDisplayChangeAllowed", creditsMeterDisplayChangeAllowed);

    const Json::Value& bangupData = aMsg["bangup"];
    objectWriter.write("egmBeginBalance", Poco::NumberFormatter::format(bangupData.get("beginBalance", 0).asUInt64()));
    objectWriter.write("egmEndBalance", Poco::NumberFormatter::format(bangupData.get("endBalance", 0).asUInt64()));
    objectWriter.write("egmBeginPaid", Poco::NumberFormatter::format(bangupData.get("beginPaid", 0).asUInt64()));
    objectWriter.write("egmEndPaid", Poco::NumberFormatter::format(bangupData.get("endPaid", 0).asUInt64()));

    std::string egmMsg = objectWriter.str();

    // send the message
    {
        v8::Locker locker;
        v8::HandleScope hs;
        v8::Handle<v8::Value> args[] =
        {
            v8::String::New(egmMsg.c_str()),
        };
        DispatchScriptEvent(&Events::mEGMEventFunc, args, 1);
    }
}

void OgreApplication::OnCreateUnityBrowser(const Json::Value& aMsg)
{
    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));

    std::string browserName = aMsg["browserName"].asString();
    std::string url = aMsg["url"].asString();
    int width = aMsg["width"].asInt();
    int height = aMsg["height"].asInt();

    pBrowserWidgetPlugin->CreateUnityBrowser(browserName, url, width, height);
}

void OgreApplication::OnDestroyUnityBrowser(const Json::Value& aMsg)
{
    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));
    pBrowserWidgetPlugin->DestroyUnityBrowser(aMsg["browserName"].asString());
}

void OgreApplication::OnUnityBrowserSetVisible(const Json::Value& aMsg)
{
    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));
    std::string browserName = aMsg["browserName"].asString();
    bool active = aMsg["active"].asBool();

    // If deactivating the techUI from the Recall page after selecting a game to recall,
    // do not reset the URL.  The mResetUnityTechUIURL will prevent
    // the resetting of the URL when returning to the techUI from a recalled game.
    // This makes sure that the techUI returns to the Recall page.
    if (!active && mUnityReplayModeActive)
    {
        mResetUnityTechUIURL = false;
    }

    pBrowserWidgetPlugin->SetUnityBrowserVisible(browserName, active, mResetUnityTechUIURL);

    // After activating the techUI from a recalled game,
    // allow the techUI URL to be reset when activating/deactivating
    // the techUI when not recalling a game.
    if (active && !mResetUnityTechUIURL)
    {
        mResetUnityTechUIURL = true;
    }

}

void OgreApplication::OnUnityBrowserMouseEvent(const Json::Value& aMsg)
{
    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));

    std::string browserName = aMsg["browserName"].asString();
    int buttonId = aMsg["buttonId"].asInt();
    int x = aMsg["x"].asInt();
    int y = aMsg["y"].asInt();
    bool pressed = aMsg["pressed"].asBool();

    pBrowserWidgetPlugin->OnUnityMouseEvent(browserName, buttonId, Ogre::Vector2(Ogre::Real(x), Ogre::Real(y)), pressed);
}

void OgreApplication::OnUnityBrowserKeyEvent(const Json::Value& aMsg)
{
    BrowserWidgetPlugin* pBrowserWidgetPlugin = (BrowserWidgetPlugin*)(getPluginByName("Plugin_BrowserWidget"));

    std::string browserName = aMsg["browserName"].asString();
    int keyId = aMsg["key"].asInt();
    bool pressed = aMsg["pressed"].asBool();

    pBrowserWidgetPlugin->OnUnityKeyboardEvent(browserName, keyId, pressed);
}

void MFORCE::GameEngine::OgreApplication::onEGMProgressiveResolutionStatus(const Json::Value& aMsg)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    if (mpMasterUI)
    {
        bool allowProgResolutionDisplay = !mpMasterUI->IsTechScreenActive();
        bool progResolutionStatus = aMsg["progressiveResolutionStatus"].asBool();

        if (allowProgResolutionDisplay && progResolutionStatus)
        {
            std::string headerMsg = MFORCE::Common::Localization::GetString("PROGRESSIVE_MESSAGE", "RESOLVING_HEADER");
            std::string bodyMsg = MFORCE::Common::Localization::GetString("PROGRESSIVE_MESSAGE", "RESOLVING_MESSAGE");
            mpMasterUI->ShowMessageBox(headerMsg, bodyMsg, MsgBoxDisplayEvent::MSGBOX_PROGRESSIVE_RESOLUTION_STATUS, 0);
        }
        else
        {
            mpMasterUI->HideMessageBox(MessageType::ProgressiveResolution);
        }
    }
}

void OgreApplication::processPatsMessages(const Json::Value& aMsg)
{
#if DEVELOPER
    PatsEngine* pPatsEngine = PatsEngine::getInstance(GetActiveThemeId());
    if (pPatsEngine)
    {
        pPatsEngine->processUnifiedMessage(aMsg);
    }
#endif
}

void OgreApplication::onQuickStop(const Json::Value& aMsg)
{
    DispatchScriptEvent(&Events::mQuickStopFunc);
}

void OgreApplication::updateMessageBox(bool aShow, const Json::Value& aMsgBoxData)
{
    Poco::ScopedLock<Poco::Mutex> locker(mUIMutex);

    const std::string& msgBoxTitle = aMsgBoxData.get("MsgTitle", "").asString();
    const std::string& msgBoxBody = aMsgBoxData.get("MsgBody", "").asString();
    int msgBoxStyle = aMsgBoxData.get("MsgStyle", 0).asInt();

    if (aShow)
    {
        ShowMessageBox(msgBoxTitle, msgBoxBody, msgBoxStyle);
    }
    else
    {
        ClearMessageBox();
    }
}

std::string OgreApplication::setupDenomSprites(const std::string& aDenomSelections, const std::vector<std::string>& aDenomList)
{
    std::string str;
    SpriteOverlayElement* sprite = NULL;
    for (int i = 0; i < aDenomList.size(); i++)
    {
        str = std::string("BaseGame/MultiDenom") + Poco::NumberFormatter::format(aDenomList.size()) + "_" + Poco::NumberFormatter::format(i + 1);
        sprite = static_cast<SpriteOverlayElement*>(OverlayManager::getSingleton().getOverlayElement(str));

        int denom = (atoi((aDenomList.at(i)).c_str()) / 1000);
        str = std::string("BaseGame/Denom_") + Poco::NumberFormatter::format(denom);
        sprite->assignMaterial(str);
    }

    return std::string("BaseGame/MultiDenom") + Poco::NumberFormatter::format(aDenomList.size());
}

void OgreApplication::sendDenomInMillicentsChangeRequest(Poco::UInt32 denom)
{
    Json::Value msg;
    Message::InitializeMessage(GROUP_GAMEENGINE, "DenomChangeRequest", Message::EVENT, msg);
    msg["denom"] = denom;
    OgreApplication::getSingleton().SendEvent(msg);

    //hide the bingo paytables
    BingoDisplayPlugin* bingoDisplayPlugin = (BingoDisplayPlugin*)(getPluginByName("Plugin_BingoDisplay"));
    if(bingoDisplayPlugin != nullptr)
    {
        bingoDisplayPlugin->HideBingoPaytables();
    }
}

void OgreApplication::setBetConfigID(Poco::UInt32 id)
{
    Json::Value msg;
    Message::InitializeMessage(GROUP_GAMEENGINE, "SetBetConfigID", Message::EVENT, msg);
    msg["id"] = id;
    SendEvent(msg);
}

void OgreApplication::OnHHRExactaBrowserControl(const std::string& url, const bool deactivate, const bool forceUnload)
{
    if (deactivate)
    {
        poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::OnHHRExactaBrowserControl: Deactivate Browsers");
        if (mpMasterUI->IsHHRExactaUIActive() || forceUnload)
        {
            mpMasterUI->DeactivateHHRExactaUI(forceUnload);
        }
    }
    else if (!url.empty())
    {
        if (mPreviousHHRExactaUrl != url)
        {
            // URL has changed so navigate to the new URL
            poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("OgreApplication::OnHHRExactaBrowserControl: Activate Browsers to %s", url));
            mpMasterUI->ActivateHHRExactaUI(url);
            mPreviousHHRExactaUrl = url;
        }
        else
        {
            // Only unhide browsers since URL is the same
            poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::OnHHRExactaBrowserControl: Unhide Browsers");
            mpMasterUI->ActivateHHRExactaUI();
        }
    }
    else
    {
        // Should not get here
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::OnHHRExactaBrowserControl: Called without URL and deactivate == false");
    }
}

void OgreApplication::addMouseListener(MouseListener* apListener)
{
    mMouseDispatcher.addListener(apListener);
}

void OgreApplication::removeMouseListener(MouseListener* apListener)
{
    mMouseDispatcher.removeListener(apListener);
}

void OgreApplication::addKeyboardListener(KeyboardListener* apListener)
{
    mKeyboardDispatcher.addListener(apListener);
}

void OgreApplication::removeKeyboardListener(KeyboardListener* apListener)
{
    mKeyboardDispatcher.removeListener(apListener);
}

void OgreApplication::onPrizeGeneratorPaytableData(const Json::Value& aMsg)
{
    MathInfo::instance().updateMathData(aMsg);
    mPrizeGenerationType = aMsg.get("PrizeGenerationType", "").asString();
    mPrizeGenerationBackEnd = aMsg.get("PrizeGenerationBackEnd", "").asString();

    Json::Value notifyData;
    notifyData["paytableData"] = true;
    CustomizedOverlayManager::getSingleton().notifyWidgets("MathData", notifyData);
}

void OgreApplication::onPrizeGeneratorReelData(const Json::Value& aMsg)
{
    MathInfo::instance().setReelData(aMsg["ReelSets"], aMsg["GameSets"], aMsg["SymbolSets"]);

    Json::Value notifyData;
    notifyData["reelData"] = true;
    CustomizedOverlayManager::getSingleton().notifyWidgets("MathData", notifyData);
}

void OgreApplication::onCriticalDriveRemoved(const Json::Value& aMsg)
{
    if (mpMasterUI)
        mpMasterUI->OnCriticalDriveRemoved(aMsg);
}

void OgreApplication::onMemoryLow(int pct, int criticalRatio)
{
#if WIN32
    unsigned long long currentTime = MicrosecondsSinceStart();

    // once every minute ? 
    if ((currentTime - mLastLowMemoryTime) > 60 * 1000 * 1000)
    {
        if (98 == pct)
        {
            mpScriptEngine->NotifyLowMemory();
            FlushGPU();
            mLastLowMemoryTime = currentTime;
        }
        else if (criticalRatio == pct)
        {
            mpScriptEngine->NotifyLowMemory();
            SimulateAltTab();
            mLastLowMemoryTime = currentTime;
        }
    }
#endif
}

void OgreApplication::onBingoCommandRequest(const Json::Value& aMsg)
{
    GameEngineMessageBroadcast::instance().broadCastMessage("Bingo", aMsg);
}

void OgreApplication::OnGPMenuCtrlMsg(const Json::Value& aMsg)
{
    GameEngineMessageBroadcast::instance().broadCastMessage(MFORCE::GP::GPCtrl, aMsg);
}

void OgreApplication::OnGPMenuLockState(const Json::Value& msg)
{
    bool showConfirmationUI = msg.get("showConfirmationUI", false).asBool();
    if (showConfirmationUI != mShowConfirmationUI)
    {
        mShowConfirmationUI = showConfirmationUI;
        // for video games
        Json::Value aMsg;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_GPMENUCONTROL, "GameNotification_ShowingConfirmationUI", MFORCE::Messaging::Message::EVENT, aMsg);
        aMsg["showConfirmationUI"] = showConfirmationUI;
        GameEngineMessageBroadcast::instance().broadCastMessage("GameNotifications", aMsg);
        
        // for mech games
        NotificationManager::Instance().PostNotification(Poco::Notification::Ptr(new ShowingConfirmationUINotification(showConfirmationUI)));
    }
}

void OgreApplication::onGameEngineEvent(const Json::Value& aMsg)
{
    GameEngineMessageBroadcast::instance().broadCastMessage("GameEngine", aMsg);
}

void OgreApplication::OnPrizeDataLoaded(const Json::Value& msg)
{
    ProgressiveDisplayPlugin* pProgressivePlugin = (ProgressiveDisplayPlugin*)(getPluginByName("Plugin_ProgressiveDisplay"));
    if (pProgressivePlugin)
    {
        pProgressivePlugin->ProcessPrizeDataLoaded(msg);
    }
}

void OgreApplication::OnLinkStatusNotification(const Json::Value& msg)
{
    bool isLinkUp = msg["isLinkUp"].asBool();
    if (isLinkUp == false)
    {
        int linkDownType = msg["last_link_down_type"].asInt();
        if (linkDownType != MFORCE::Cabinet::LinkDownType::None)
        {
            mLinkDownCounts[linkDownType]++;
        }
    }
}

void OgreApplication::onProgressivesEvent(const Json::Value& msg)
{
    ProgressiveDisplayPlugin* pProgressivePlugin = (ProgressiveDisplayPlugin*)(getPluginByName("Plugin_ProgressiveDisplay"));
    if (pProgressivePlugin)
    {
        pProgressivePlugin->ProcessEvent(msg);
    }

    GameEngineMessageBroadcast::instance().broadCastMessage("Progressives", msg);
}

void OgreApplication::OnProgressiveSettingsChanged(const Json::Value &msg)
{
    if (mpDemoMenuUI)
    {
        mpDemoMenuUI->OnProgressiveSettingsChanged(msg);
    }
}

void OgreApplication::OnSetTierValueEvent(const Json::Value& msg)
{
    ProgressiveDisplayPlugin* pProgressivePlugin = (ProgressiveDisplayPlugin*)(getPluginByName("Plugin_ProgressiveDisplay"));
    if (pProgressivePlugin)
    {
        pProgressivePlugin->ProcessSetTierValueEvent(msg);
    }
}

void OgreApplication::OnResetTierEvent(const Json::Value& msg)
{
    ProgressiveDisplayPlugin* pProgressivePlugin = (ProgressiveDisplayPlugin*)(getPluginByName("Plugin_ProgressiveDisplay"));
    if (pProgressivePlugin)
    {
        pProgressivePlugin->ProcessResetTierEvent(msg);
    }
}

void OgreApplication::onAttributeChanged(const Json::Value& aMsg)
{
    const std::string& moduleName = aMsg.get("moduleName", "_invalid_").asString();

    if (moduleName == MODULENAME_GAMEENGINE)
    {
        CustomizedOverlayManager::getSingleton().notifyWidgets("AttributeChanged", aMsg);
    }

    GameEngineMessageBroadcast::instance().broadCastMessage("Bingo", aMsg);
}

void OgreApplication::OnEnterSitAndGoTournamentModeEvent(const Json::Value& /*msg*/)
{
    mIsInSitAndGoTransitionArc = true;
}

void OgreApplication::onTournamentModeEntered()
{
    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for(auto entry : plugins)
    {
        PluginOnEnterTournamentMode(entry);
    }
}

void OgreApplication::onTournamentModeExited()
{
    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for(auto entry : plugins)
    {
        PluginOnExitTournamentMode(entry);
    }
}

void OgreApplication::onPluginLoadResources(const Json::Value& msg)
{
    std::string pluginName = msg.get("pluginName", "").asString();

    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for(auto plugin : plugins)
    {
        ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
        if(scriptablePlugin)
        {
            if(scriptablePlugin->getName() == pluginName)
            {
                scriptablePlugin->LoadResources(pluginName);
            }
        }
    }
}

void OgreApplication::onPluginUnLoadResources(const Json::Value& msg)
{
    std::string pluginName = msg.get("pluginName", "").asString();

    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    for(auto plugin : plugins)
    {
        ScriptablePlugin* scriptablePlugin = dynamic_cast<ScriptablePlugin*>(plugin);
        if(scriptablePlugin)
        {
            scriptablePlugin->UnLoadResources(pluginName);
        }
    }
}

void OgreApplication::OnSitAndGoLoadingScene(const Json::Value& msg)
{
    const std::string themeId = msg.get("themeId", "").asString();
    const bool start = msg.get("start", false).asBool();
    const bool resetLoadingScene = msg.get("resetLoadingScene", false).asBool();
    if (start)
    {
        mShowSitAndGoLoadingScene = true;
        mpMasterUI->DisplayUnavailableScreens(false);
        if (themeId.empty())
        {
            // Exit SitAndGo game, set active themeId for loading scene as active game's themeId.
            mpIdleScene->SetActiveThemeId(mActiveGame.ThemeId);
        }
        else
        {
            // Enter SitAndGo game, set active themeId for loading scene as input themeId.
            mpIdleScene->SetActiveThemeId(themeId);
        }
        mpIdleScene->ShowLoadingOverlays(false);
        mpIdleScene->ShowLoadingScene(true);
    }
    else
    {
        mShowSitAndGoLoadingScene = false;
        if (mpIdleScene)
        {
            mpIdleScene->SetActiveThemeId("");
            if (resetLoadingScene)
            {
                mpIdleScene->ShowLoadingOverlays(false);
                mpIdleScene->ShowLoadingScene(false);
            }
        }
    }
}

Ogre::Root* OgreApplication::getRoot()
{
    return mRoot;
}

Poco::Random& OgreApplication::getRandom()
{
    return mRandom;
}

Autoplayer& OgreApplication::getAutoplayer()
{
    return *(mAutoplayer.get());
}

Ogre::Real OgreApplication::getTimeBaseDelta() const
{
    return mTimebaseDelta;
}

unsigned int OgreApplication::getCurrentFPS() const
{
    return (unsigned int)mCurrentFPS;
}

void OgreApplication::evictManagedResources()
{
#if WIN32
    mpOgreLog->logMessage("Evicting managed resources", Ogre::LML_NORMAL);
    IDirect3DDevice9* d3ddevice;
    for (WindowMap::iterator it = mWindows.begin(); it != mWindows.end(); ++it)
    {
        d3ddevice = NULL;
        (*it).second->getCustomAttribute("D3DDEVICE", &d3ddevice);
        if (d3ddevice)
        {
            ((IDirect3DDevice9*)d3ddevice)->EvictManagedResources();
        }
    }
#endif
}

void OgreApplication::takeScreenshots(const Poco::Path& aPath)
{
    WindowMap::iterator itWindows = mWindows.begin();
    while (itWindows != mWindows.end())
    {
        RenderWindow* pWindow = (*itWindows).second;

        std::string ssFile = aPath.toString();
        ssFile.append(pWindow->getName());
        ssFile.append("_");

#if DEVELOPER
        MemoryRenderBuffer* pMemRenderBuffer = GetSnapshotMemoryRenderBufferForScreen(pWindow->getName());
        if (pMemRenderBuffer)
        {
            pMemRenderBuffer->WriteContentsToTimestampedFile(ssFile, ".jpg");
        }
        else
        {
#endif
            pWindow->writeContentsToTimestampedFile(ssFile, ".jpg");

#if DEVELOPER
        }
#endif

        ++itWindows;
    }
}

void OgreApplication::takeScreenshots(const Poco::Path& aPath, const std::string& strFileName, const std::string& strWindowName)
{
    WindowMap::iterator itWindows = mWindows.begin();
    while (itWindows != mWindows.end())
    {
        RenderWindow* pWindow = (*itWindows).second;

        std::string ssFile = aPath.toString();
        std::string strWindow = pWindow->getName();

        if (strWindow == strWindowName || strWindowName == "")
        {
            ssFile.append(strWindow);
            ssFile.append("_");
            ssFile.append(strFileName);
            ssFile.append(".jpg");

            pWindow->writeContentsToFile(ssFile);
        }

        ++itWindows;
    }
}


void OgreApplication::onScreenshot(const Json::Value& msg)
{

    const std::string& strPath = msg.get("Path", "").asString();
    const std::string& strFileName = msg.get("FileName", "").asString();
    const std::string& strWindowName = msg.get("WindowName", "").asString();

    Poco::Path ssPath(strPath);
    if (strPath == "")
    {
        PathInfo::Instance()->GetPath(PathInfo::ScreenshotPathID, ssPath);
    }

    if (strFileName == "")
    {
        takeScreenshots(ssPath);
    }
    else
    {
        takeScreenshots(ssPath, strFileName, strWindowName);
    }

    Json::Value replyMsg;
    MFORCE::Messaging::Message::InitializeMessage(GROUP_GAMEENGINE,
        "ScreenshotTaken",
        MFORCE::Messaging::Message::EVENT,
        replyMsg);
    replyMsg["ID"] = msg.get("ID", "");
    SendEvent(replyMsg);
}

bool OgreApplication::isBingoGame() const
{
    return (Poco::icompare(mCentralType, "bingo") == 0);
}

bool OgreApplication::isHHRExactaGame() const
{
    return (Poco::icompare(mCentralType, "HHR_Exacta") == 0);
}

bool OgreApplication::isReelsFirstGame() const
{
    return mPrizeGenerationType == "ReelsFirst";
}

bool OgreApplication::isPickBonusRevealEnabled() const
{
    DataLayer::Attributes attributes;
    bool enablePickBonusReveal = true;
    attributes.getConfigValue("EGM.EnablePickBonusReveal", enablePickBonusReveal);
    return (enablePickBonusReveal == true);
}

bool OgreApplication::isDigitalMechReelsEnabled() const
{
    DataLayer::Attributes attributes;
    bool enableDigitalMechReels = false;
    attributes.getConfigValue("EGM.DigitalMechReelsEnabled", enableDigitalMechReels);
    return (enableDigitalMechReels == true);
}

bool OgreApplication::areAggregatePrizesAllowedToBeShown() const
{
    DataLayer::Attributes attributes;
    bool aggregatePrizesAllowedToBeShown = true;
    attributes.getConfigValue("EGM.AggregatePrizesDisplay", aggregatePrizesAllowedToBeShown);
    return aggregatePrizesAllowedToBeShown;
}

Poco::UInt32 OgreApplication::getRenderQueueTime() const
{
    return (Poco::UInt32) mFrameRenderQueueTime;
}

Poco::UInt32 OgreApplication::getRenderQueueCount() const
{
    return mFrameRenderQueueCount;
}

Poco::UInt32 OgreApplication::getRenderObjectCount() const
{
    return mFrameRenderObjectCount;
}

void OgreApplication::preRenderQueues()
{
    if (mbShowOverdraw)
    {
        if (mOverdrawPass)
            mSceneMgr->_setPass(mOverdrawPass, true);
    }
    else
    {
        ++mRenderQueuePassCount;
    }
}

void OgreApplication::postRenderQueues()
{
}

void OgreApplication::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
{
    if (!mbShowOverdraw)
    {
        mRenderQueueInfo.queueGroupId = queueGroupId;
        mRenderQueueInfo.invocation = invocation;
        mRenderQueueInfo.objectList.clear();
        mRenderQueueInfo.time = MicrosecondsSinceStart();
    }
}

void OgreApplication::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
{
    if (!mbShowOverdraw)
    {
        if ((mRenderQueueInfo.queueGroupId != queueGroupId) || (mRenderQueueInfo.invocation != invocation))
        {
            Ogre::String rqmsg = Poco::format("RenderQueue end [%?d, '%s'] does not match start [%?d, '%s']",
                mRenderQueueInfo.queueGroupId, mRenderQueueInfo.invocation, queueGroupId, invocation);
            LogManager::getSingleton().logMessage(rqmsg, Ogre::LML_CRITICAL);
            return;
        }

        mRenderQueueInfo.time = MicrosecondsSinceStart() - mRenderQueueInfo.time;  // delta time

        mRenderQueueTime += mRenderQueueInfo.time;  // accumulate render queue time for frame

        // don't bother saving render queue detail if queue had no renderables
        if (!mRenderQueueInfo.objectList.empty())
            mRenderQueueInfoForFrame.push_back(mRenderQueueInfo);

        ++mRenderQueueCount;
    }
}

void OgreApplication::notifyRenderSingleObject(Ogre::Renderable *pRend, const Ogre::Pass *pPass,
    const Ogre::AutoParamDataSource *pSource, const Ogre::LightList *pLightList,
    bool suppressRenderStateChanges)
{
    RenderObjectInfo renderObjectInfo;

#ifdef DEVELOPER
    // expensive Renderable type determination only done for developer builds
    Ogre::OverlayElement *pOverlayElement = dynamic_cast<Ogre::OverlayElement *>(pRend);
    if (pOverlayElement != NULL)
    {
        renderObjectInfo.name = pOverlayElement->getName();
        renderObjectInfo.type = pOverlayElement->getTypeName();
    }
    else
    {
        renderObjectInfo.name = "";
#if WIN32
        if (dynamic_cast<TextRenderable *>(pRend) != NULL)
        {
            renderObjectInfo.type = "text";
        }
        else
#endif
            if (dynamic_cast<Ogre::BorderRenderable *>(pRend) != NULL)
            {
                renderObjectInfo.type = "border";
            }
            else
            {
                renderObjectInfo.type = "?";
            }
    }
#else
    renderObjectInfo.name = "";
    renderObjectInfo.type = "N/A";
#endif

    const Ogre::MaterialPtr pMaterial = pRend->getMaterial().staticCast<Ogre::Material>();
    renderObjectInfo.material = pMaterial.isNull() ? "?" : pMaterial->getName();

    renderObjectInfo.target = pSource->getCurrentRenderTarget()->getName();

    // add object info to current render queue data
    mRenderQueueInfo.objectList.push_back(renderObjectInfo);

    ++mRenderObjectCount;
}

void OgreApplication::PrintRenderQueueData()
{
    RenderQueueInfoForFrame renderQueueData = mFrameRenderQueueInfo;

    for (RenderQueueInfoForFrame::const_iterator it = renderQueueData.begin(); it != renderQueueData.end(); ++it)
    {
        size_t objectCount = it->objectList.size();
        if (objectCount > 0)
        {
            std::string target = it->objectList[0].target;

            Poco::UInt32 groupId = (Poco::UInt32) (it->queueGroupId);
            std::string invocaton = it->invocation;
            Poco::UInt64 time = it->time;

            std::string groupName = "";
            switch (groupId)
            {
            case   0: groupName = " (Background)"; break;
            case  50: groupName = " (Main)";       break;
            case  95: groupName = " (Widgets)";    break;
            case 100: groupName = " (Overlays)";   break;
            }

            std::string logline = Poco::format("Render queue group %?d%s for %s took %?d microseconds for the following objects:\n",
                groupId, groupName, target, time);

            for (size_t j = 0; j < objectCount; ++j)
            {
                std::string name = it->objectList[j].name;
                std::string namefield = name.empty() ? "" : ", name: " + name;

                std::string mat = it->objectList[j].material;
                std::string matfield = mat.empty() ? "" : ", material: " + mat;

                std::string objtarget = it->objectList[j].target;
                std::string targetfield = (objtarget == target) ? "" : ", mismatched target: " + objtarget;

                logline += Poco::format("%?d - type: %s%s%s%s\n", j, it->objectList[j].type, matfield, namefield, targetfield);
            }

            mpOgreLog->logMessage(logline, LML_CRITICAL);
        }
    }
}

void OgreApplication::setRenderTarget(const std::string& aRenderWindow)
{
    RenderWindow* pWindow = getRenderWindow(aRenderWindow);
    if (pWindow != NULL)
    {
        Root::getSingleton().getRenderSystem()->_setRenderTarget(pWindow);
    }
}

void OgreApplication::createStockShaders()
{
    /////////////////////////////////////////////////////////////////////////////// 
    ///
    ///   YUV Conversion Shaders - Used by video plugins, bink, etc.
    ///
    ///////////////////////////////////////////////////////////////////////////////
#define YUV2RGB_SHADER_SOURCE "\
    float4 main_fp(\
    in float3 uvY : TEXCOORD0,\
    in float4 vertColor: COLOR0, \
    uniform float3 scaleVal,\
    uniform float colorIntensity,\
    uniform sampler2D txY,\
    uniform sampler2D txU,\
    uniform sampler2D txV ) : COLOR\
    {\
    const float4 crc = { 1.595794678f, -0.813476563f, 0, 0.0 };\
    const float4 crb = { 0, -0.391448975f, 2.017822266f, 0.0 };\
    const float4 adj = { -0.87065506f, 0.529705048f, -1.081668854f, 0 };\
    float4 p;\
    \
    float y = tex2D( txY, uvY * scaleVal ).a;\
    float cb = tex2D( txU, uvY * scaleVal ).a;\
    float cr = tex2D( txV, uvY * scaleVal ).a;\
    \
    p = y * 1.164123535f;\
    \
    p += ((crc * cr) + (crb * cb) + adj) * colorIntensity;\
    \
    p.a = 1.0f;\
    \
    p *= vertColor;\
    \
    return p;\
    }"

#define YUV2RGBA_SHADER_SOURCE "\
    float4 main_fp(\
    uniform float3 scaleVal,\
    uniform float colorIntensity,\
    in float3 uvY : TEXCOORD0,\
    in float4 vertColor: COLOR0, \
    uniform sampler2D txY,\
    uniform sampler2D txU,\
    uniform sampler2D txV,\
    uniform sampler2D txA) : COLOR\
    {\
    const float4 crc = { 1.595794678f, -0.813476563f, 0, 0.0 };\
    const float4 crb = { 0, -0.391448975f, 2.017822266f, 0.0 };\
    const float4 adj = { -0.87065506f, 0.529705048f, -1.081668854f, 0 };\
    float4 p;\
    \
    float y = tex2D( txY, uvY * scaleVal ).a;\
    float cb = tex2D( txU, uvY * scaleVal ).a;\
    float cr = tex2D( txV, uvY * scaleVal ).a;\
    \
    p = y * 1.164123535f;\
    \
    p += ((crc * cr) + (crb * cb) + adj) * colorIntensity;\
    \
    p.a = tex2D( txA, uvY * scaleVal ).a;\
    \
    p *= vertColor;\
    \
    return p;\
    }"

    /////////////////////////////////////////////////////////////////////////////// 

    /// Setup shaders used for color conversion
    //This shader does not process alpha channel
    mpYUV2RGBShader = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
        YUV2RGB_SHADER,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        "cg",
        Ogre::GPT_FRAGMENT_PROGRAM);

    mpYUV2RGBShader->setSource(YUV2RGB_SHADER_SOURCE);
    mpYUV2RGBShader->setParameter("entry_point", "main_fp");
    mpYUV2RGBShader->setParameter("profiles", "ps_2_0 arbfp1");
    mpYUV2RGBShader->load();

    //This shader does process alpha channel
    mpYUV2RGBAShader = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
        YUV2RGBA_SHADER,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        "cg",
        Ogre::GPT_FRAGMENT_PROGRAM);

    mpYUV2RGBAShader->setSource(YUV2RGBA_SHADER_SOURCE);
    mpYUV2RGBAShader->setParameter("entry_point", "main_fp");
    mpYUV2RGBAShader->setParameter("profiles", "ps_2_0 arbfp1");
    mpYUV2RGBAShader->load();

#if DEVELOPER
    if (mSeeThruMgr)
    {
         mSeeThruMgr->CreateShaders();
    }
#endif
}

void OgreApplication::releaseStockShaders()
{
    mpYUV2RGBShader->unload();
    mpYUV2RGBAShader->unload();
    mpYUV2RGBShader.setNull();
    mpYUV2RGBAShader.setNull();
}

Ogre::Plugin* OgreApplication::getPluginByName(const std::string& aPluginName) const
{
    const Ogre::Root::PluginInstanceList& installedPlugins = Ogre::Root::getSingleton().getInstalledPlugins();

    for (int i = 0; i < installedPlugins.size(); i++)
    {
        if (installedPlugins[i]->getName() == aPluginName)
        {
            return installedPlugins[i];
        }
    }

    return NULL;
}

void OgreApplication::setHintsAllowed(bool aAllowed)
{
    mHintsAllowed = aAllowed;
}

bool OgreApplication::getHintsAllowed() const
{
    return mHintsAllowed;
}

void OgreApplication::setBonusGaffing(bool enabled)
{
    mBonusGaffing = enabled;
}

bool OgreApplication::getBonusGaffing() const
{
    return mBonusGaffing && !isInReplayMode() && !mUnityReplayModeActive;
}

bool OgreApplication::allowShowDemoOverlay() const
{
    // allow ShowDemoOverlay if demo is enabled and hints is turned on
    DataLayer::Attributes attributes;
    bool demoMenuEnabled = false;
    attributes.getConfigValue("gameengine.EnableDemoMenu", demoMenuEnabled);

    return (demoMenuEnabled && mHintsAllowed);
}

void OgreApplication::writeV8HeapSnapshot()
{
    mpScriptEngine->HeapSnapshot();
}

Poco::UInt32 OgreApplication::getFrameHitchThreshold() const
{
    return mFrameHitchThreshold;
}

Poco::UInt32 OgreApplication::getFrameHitchTrapThreshold() const
{
    return mFrameHitchTrapThreshold;
}

void OgreApplication::setFrameHitchThreshold(const Poco::UInt32 threshold)
{
    if (mFrameHitchThreshold < mDefaultFPS)
    {
        mFrameHitchThreshold = threshold;
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("FrameHitchThreshold set to %?u", threshold));
    }
}

void OgreApplication::resetHitchCount()
{
    mHitchCount = 0;
}

Poco::UInt32 OgreApplication::getHitchCount() const
{
    return mHitchCount;
}

const std::vector<Poco::UInt32>& OgreApplication::getLinkDownCounts() const
{
    return mLinkDownCounts;
}

Poco::UInt32 OgreApplication::getEvictCount() const
{
    return mEvictCount;
}

Poco::UInt32 OgreApplication::getGarbageCollectAllCount() const
{
    return mpScriptEngine->GetGCAllCount();
}

Poco::UInt32 OgreApplication::getGarbageCollectScavengeCount() const
{
    return mpScriptEngine->GetGCScavengeCount();
}

Poco::UInt32 OgreApplication::getGarbageCollectMarkSweepCount() const
{
    return mpScriptEngine->GetGCMarkSweepCompactCount();
}

Poco::UInt32 OgreApplication::getTotalHeapSize() const
{
    v8::HeapStatistics heapStats;
    v8::V8::GetHeapStatistics(&heapStats);
    return (Poco::UInt32)heapStats.total_heap_size();
}

Poco::UInt32 OgreApplication::getUsedHeapSize() const
{
    v8::HeapStatistics heapStats;
    v8::V8::GetHeapStatistics(&heapStats);
    return (Poco::UInt32)heapStats.used_heap_size();
}

ScriptObjectCountMap OgreApplication::GetScriptObjectCounts() const
{
    return mpScriptEngine->GetScriptObjectCounts();
}

unsigned int OgreApplication::getTotalScriptObjectCount() const
{
    return mpScriptEngine->GetTotalScriptObjectCount();
}

void OgreApplication::ResetTheme()
{
    mActiveGame.Clear();
}

void OgreApplication::onGameFinishedUnloading()
{
    if (mIsButtonPanelRescaled)
    {
        ScaleButtonPanelAndPositionCamera(1.0f, 1.0f);  //restore button panel scaling to original if it was rescaled
          // let GP Menu plugin know if BP was restored
        Json::Value rescaleNotification;
        MFORCE::Messaging::Message::InitializeMessage(GROUP_GPMENUCONTROL, MFORCE::GP::GPCtrl, MFORCE::Messaging::Message::EVENT, rescaleNotification);
        rescaleNotification[MFORCE::GP::GPField_Cmd] = MFORCE::GP::GPEvent_ButtonPanelScaling;
        rescaleNotification[MFORCE::GP::GPField_ButtonPanelScaled] = false;
        OnGPMenuCtrlMsg(rescaleNotification);
    }

    evictManagedResources();

    // notify internal entities
    NotificationManager::Instance().PostNotification(new MFORCE::GamePlay::GameUnLoadedNotification(mActiveGame));
    // notify the rest of the system
    Json::Value dataUnloaded;
    Message::InitializeMessage(GROUP_SYSTEM, "GameDataUnloadedNotification", Message::EVENT, dataUnloaded);
    dataUnloaded["themeName"] = mActiveGame.ThemeName;
    dataUnloaded["themeId"] = mActiveGame.ThemeId;
    dataUnloaded["gamePlayId"] = mActiveGame.Id;
    dataUnloaded["moduleName"] = MODULENAME_GAMEENGINE;
    dataUnloaded["success"] = true;
    SendEvent(dataUnloaded);

    mpOgreLog->logMessage(String("Game unloaded: " + mActiveGame.ThemeName));

    // save mActiveGame.ThemeName since it will be "" after ResetTheme()
    std::string themeName = mActiveGame.ThemeName;

    delinkComponentsOfThemeId(mActiveGame.ThemeId);

    ResetTheme();
    poco_notice(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Game %s unloaded successfully.", themeName));
    // force full gc
    if (mpScriptEngine)
    {
        mpScriptEngine->NotifyLowMemory();
    }
}

DataLayer::ThemeComponentList OgreApplication::GetComponentsToLoad(const std::string &themeId)
{
    DataLayer::ThemeComponentList compList;
    if (DataLayer::Repository::Instance().GetGamePlayDataSource()->HasTheme(themeId))
    {
        compList = DataLayer::Repository::Instance().GetGamePlayDataSource()->GetThemeComponents(themeId);
    }

    DataLayer::ThemeComponentList systemCompList = GetSystemComponents(themeId);

    compList.insert(compList.end(), systemCompList.begin(), systemCompList.end());

    return compList;
}

DataLayer::ThemeComponentList OgreApplication::GetSystemComponents(const std::string &themeId)
{
    DataLayer::ThemeComponentList compList;

    AddComponentForMonitor("ButtonPanel", "BUTTON_PANEL", themeId, compList);

    DataLayer::ThemeComponentList emulatorCompList = GetEmulatorComponents(themeId);

    compList.insert(compList.end(), emulatorCompList.begin(), emulatorCompList.end());

    return compList;
}

DataLayer::ThemeComponentList OgreApplication::GetEmulatorComponents(const std::string &themeId)
{
    DataLayer::ThemeComponentList compList;

    AddComponentForMonitor("MechReelsEmulator", "MECH_REELS", themeId, compList);
    AddComponentForMonitor("MarqueeEmulator", "MARQUEE", themeId, compList);

    return compList;
}

void OgreApplication::AddComponentForMonitor(const std::string &compName,
    const std::string &monitorName,
    const std::string &themeId,
    DataLayer::ThemeComponentList &compList)
{
    auto componentOwnersIter = mComponentOwners.find(compName);
    if (componentOwnersIter != mComponentOwners.end() || themeId.empty())
    {
        // Check if the requesting theme ID is a component owner
        if (themeId.empty() || 
            std::find(componentOwnersIter->second.begin(), componentOwnersIter->second.end(), themeId) != componentOwnersIter->second.end())
        {
            const MonitorConfig& monitorConfig = MonitorConfig::Instance();
            const MonitorConfig::MonitorDataContainer& monitors = monitorConfig.GetMonitors();
            MonitorConfig::MonitorDataContainer::const_iterator monitorItr =
                MFORCE::STLExtensions::find_if(MFORCE::STLExtensions::MakeIseq(monitors), FindByName(monitorName));
            if (monitorItr != monitors.end())
            {
                // ensure no conflict between system component and game component
                DataLayer::ThemeComponentList::const_iterator tcItr =
                    MFORCE::STLExtensions::find_if(MFORCE::STLExtensions::MakeIseq(compList), FindByName(compName));
                if (tcItr == compList.end())
                {
                    // Create a theme component def
                    DataLayer::ThemeComponent tc;
                    tc.Name = compName;
                    tc.IsBaseComponent = false;
                    tc.Path = "[Components]/common/" + Poco::toLower(compName);

                    // Add it to the list to be built
                    compList.push_back(tc);
                }
            }
        }
    }
}

void OgreApplication::onCreateTextField(const Poco::AutoPtr<MFORCE::CreateTextFieldActorNotification>& notification)
{
    TextFieldActorMap::const_iterator it = mTextFieldActors.find(notification->TextFieldId);
    if (mTextFieldActors.end() == it && !notification->TextFieldId.empty())
    {
        TextOverlayElement* overlayText = GraphicUtil::getText(notification->TextFieldId);
        if (notification->bSetDefaultCaption)
        {
            if (overlayText != nullptr)
            {
                overlayText->setOriginalCaption(notification->DefaultCaption);
                overlayText->setCaption(notification->DefaultCaption);
            }
        }
        mTextFieldActors.insert(std::make_pair(
            notification->TextFieldId,
            TextFieldActorPtr(
                new TextFieldActor(notification->TextFieldId, Poco::Logger::get(MODULENAME_GAMEENGINE), overlayText, notification->DefaultLabelId, assembleOverlayMap(notification->LabelIds))
            )
        ));
    }
}

void OgreApplication::toggleOverdraw(bool bForceClose)
{
    if (!mHibernating)
    {
        mbShowOverdraw ^= true;

        if (bForceClose)
        {
            mbShowOverdraw = false;
        }

        if (mbShowOverdraw)
        {
            Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().create("Core/Overdraw", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).staticCast<Ogre::Material>();
            mOverdrawPass = m->createTechnique()->createPass();
            Ogre::TextureUnitState* pState = mOverdrawPass->createTextureUnitState();

            m->setLightingEnabled(false);
            m->setDepthCheckEnabled(false);
            m->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
            pState->setAlphaOperation(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, 0.10f);
            pState->setColourOperationEx(Ogre::LBX_SOURCE1, Ogre::LBS_MANUAL, Ogre::LBS_CURRENT, Ogre::ColourValue(1.0f, 0.75f, 0.0f));

            mSceneMgr->_suppressRenderStateChanges(true);
            mSceneMgr->addRenderQueueListener(this);
            CompositorDirector::getSingleton().pauseCompositors();

        }
        else
        {
            mSceneMgr->removeRenderQueueListener(this);
            mSceneMgr->_suppressRenderStateChanges(false);
            Ogre::MaterialManager::getSingleton().remove("Core/Overdraw");
            CompositorDirector::getSingleton().resumeCompositors();
        }
    }
}

void OgreApplication::toggleRenderQueueStats()
{
    if (!mHibernating)
    {
        mTrackRenderQueueStats ^= true;
        if (mTrackRenderQueueStats)
        {
            mSceneMgr->addRenderQueueListener(this);
            mSceneMgr->addRenderObjectListener(this);
        }
        else
        {
            mSceneMgr->removeRenderQueueListener(this);
            mSceneMgr->removeRenderObjectListener(this);
        }
    }
}

const MathInfo& OgreApplication::getMathInfo() const
{
    return MathInfo::instance();
}

void OgreApplication::addGameEngineFrameListener(UpdaterManager::Updater aUpdater, Ogre::FrameListener* aListener)
{
    mUpdaterManager.addFrameListener(aUpdater, aListener);
}

void OgreApplication::removeGameEngineFrameListener(UpdaterManager::Updater aUpdater, Ogre::FrameListener* aListener)
{
    mUpdaterManager.removeFrameListener(aUpdater, aListener);
}

void OgreApplication::addOgreAppEventListener(OgreApplicationEventListener* eventListener)
{
    if (NULL != eventListener)
    {
        mpListeners.push_back(eventListener);
    }
    else
    {
        mpOgreLog->logMessage("Attempting to add null \"OgreApplicationEventListener\"", LML_CRITICAL);
    }
}

void OgreApplication::removeOgreAppEventListener(OgreApplicationEventListener* eventListener)
{
    mpListeners.remove(eventListener);
}

void OgreApplication::show(const MessageType& msgType,
    const std::string& title,
    const std::string& message,
    const std::string& messageDesc,
    const DialogType& dialogType,
    Poco::UInt64 displayDuration)
{
    if (NULL != mpMasterUI)
        mpMasterUI->ShowMessageBox(msgType, title, message, messageDesc, dialogType, displayDuration);
}

void OgreApplication::hide()
{
    if (NULL != mpMasterUI)
        mpMasterUI->HideMessageBox();
}

void OgreApplication::hide(const MessageType& msgType)
{
    if (NULL != mpMasterUI)
        mpMasterUI->HideMessageBox(msgType);
}

bool OgreApplication::isMessageBoxShowing()
{
    if (NULL != mpMasterUI)
    {
        return mpMasterUI->IsMessageBoxActive();
    }
    else
    {
        return false;
    }
}
Json::Value OgreApplication::getShortcutData() const
{
    return mKeyboardShortcuts.getKeyData();
}

void OgreApplication::increaseHitchThreshold()
{
    if (mFrameHitchTrapThreshold != 0)
    {
        if ((mFrameHitchTrapThreshold + FRAME_HITCH_TRAP_THRESHOLD_INTERVAL) <= 1000)
        {
            mFrameHitchTrapThreshold += FRAME_HITCH_TRAP_THRESHOLD_INTERVAL;
            setFrameHitchTrap();
        }
    }
    else if (mFrameHitchThreshold < mTargetFPS - 1)
    {
        ++mFrameHitchThreshold;
        mpOgreLog->logMessage(Poco::format("Frame hitch detection threshold set to %?d FPS", mFrameHitchThreshold));
    }
}

void OgreApplication::decreaseHitchThreshold()
{
    if (mFrameHitchTrapThreshold != 0)
    {
        if (mFrameHitchTrapThreshold > FRAME_HITCH_TRAP_THRESHOLD_INTERVAL)
        {
            mFrameHitchTrapThreshold -= FRAME_HITCH_TRAP_THRESHOLD_INTERVAL;
            setFrameHitchTrap();
        }
    }
    else if (mFrameHitchThreshold > 0)
    {
        --mFrameHitchThreshold;
        mpOgreLog->logMessage(Poco::format("Frame hitch detection threshold set to %?d FPS", mFrameHitchThreshold));
    }
}

void OgreApplication::toggleFrameHitchTrap()
{
    if (mFrameHitchTrapThreshold != 0)
    {
        mFrameHitchTrapThreshold = 0;
    }
    else
    {
        // Set the default trap interval in ms, that is 2 FPS => 500 ms
        mFrameHitchTrapThreshold = 500;
    }

    setFrameHitchTrap();
}

void OgreApplication::hideFPSDisplay()
{
    if (mpFPSOverlay == NULL)
    {
        return;
    }

    Ogre::Viewport* pVp = getPrimaryViewport();
    CustomizedOverlayManager::getSingletonPtr()->hideOverlay(pVp, mpFPSOverlay);
    mpFPSOverlay->hide();
    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
    om.destroy(mpFPSOverlay);
    om.destroyOverlayElement(mpFPSText);
    om.destroyOverlayElement(mpFPSGraph);
    om.destroyOverlayElement("Core/FPSPanel");
    mpFPSGraph = NULL;
    mpFPSOverlay = NULL;
    mpFPSText = NULL;
}

void OgreApplication::toggleFPSDisplay()
{
    if (mpFPSOverlay == NULL && !mHibernating)
    {
        Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
        mpFPSOverlay = om.create("Core/FPSOverlay");
        mpFPSOverlay->setZOrder(650);

        Ogre::Viewport* pVp = getPrimaryViewport();
        int width = pVp->getActualWidth();
        Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(om.createOverlayElement("Panel", "Core/FPSPanel"));
        panel->setMetricsMode(Ogre::GMM_PIXELS);
        panel->setPosition(5.0f, 5.0f);
        panel->setDimensions((Ogre::Real)(width - 10), 270.0f);
        panel->setMaterialName("Core/StatsBlockCenter");
        mpFPSOverlay->add2D(panel);

        mpFPSText = static_cast<Ogre::TextAreaOverlayElement*>(om.createOverlayElement("TextArea", "Core/FPSTXT"));
        mpFPSText->setMetricsMode(Ogre::GMM_PIXELS);
        mpFPSText->setPosition(5.0f, 5.0f);
        mpFPSText->setDimensions(40.0f, 40.0f);
        mpFPSText->setCharHeight(32.0f);
        mpFPSText->setFontName("MessagePopupText");
        mpFPSText->setColour(Ogre::ColourValue());
        panel->addChild(mpFPSText);

        mpFPSGraph = dynamic_cast<Ogre::LineStreamOverlayElement *>(OverlayManager::getSingleton().createOverlayElement("LineStream", "Core/FPSGraph"));
        mpFPSGraph->setMetricsMode(GMM_PIXELS);
        mpFPSGraph->setPosition(80.0f, 50.0f);
        mpFPSGraph->setDimensions((Ogre::Real)(width - 160), 200.0f);
        mpFPSGraph->setNumberOfTraces(5);

        mpFPSGraph->setNumberOfSamplesForTrace(0, 1);   // to draw 60 fps line
        mpFPSGraph->setNumberOfSamplesForTrace(1, 1);   // to draw 30 fps line
        mpFPSGraph->setNumberOfSamplesForTrace(2, 300); // ~last 5 seconds at 60 fps
        mpFPSGraph->setNumberOfSamplesForTrace(3, 300);
        mpFPSGraph->setNumberOfSamplesForTrace(4, 300);
        mpFPSGraph->createVertexBuffer();
        if (mbFrameTime)
        {
            mpFPSGraph->setEntireTrace(0, 33.333f);
            mpFPSGraph->setEntireTrace(1, 16.667f);
        }
        else
        {
            mpFPSGraph->setEntireTrace(0, 60.0f);
            mpFPSGraph->setEntireTrace(1, 30.0f);
        }
        mpFPSGraph->setTraceColor(0, Ogre::ColourValue::Blue);
        mpFPSGraph->setTraceColor(1, Ogre::ColourValue::Red);
        if (mbSmoothFrames)
            mpFPSGraph->setTraceColor(2, Ogre::ColourValue::Green);
        else
            mpFPSGraph->setTraceColor(2, Ogre::ColourValue::White);
        Ogre::ColourValue color;
        color.setAsARGB(Ogre::ARGB(4290794751));
        mpFPSGraph->setTraceColor(3, color);
        color.setAsARGB(Ogre::ARGB(4288266495));
        mpFPSGraph->setTraceColor(4, color);
        mpFPSGraph->setMaterialName("BaseWhiteNoLighting");
        panel->addChild(mpFPSGraph);

        CustomizedOverlayManager::getSingletonPtr()->showOverlay(pVp, mpFPSOverlay);
    }
    else
    {
        hideFPSDisplay();
    }
}

void MFORCE::GameEngine::OgreApplication::toggleProfiling()
{
#if DEVELOPER
    if (mbRecordAllFrames)
    {
        mbProfilerEnabled = false;
        mbRecordAllFrames = false;
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Profiling logging is off");
    }
    else if (mbProfilerEnabled && !mbRecordAllFrames)
    {
        mbProfilerEnabled = true;
        mbRecordAllFrames = true;
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Profiling logging is on and logging all frames");
    }
    else
    {
        mbProfilerEnabled = true;
        mbRecordAllFrames = false;
        poco_warning(Poco::Logger::get(MODULENAME_GAMEENGINE), "Profiling logging is on and logging only frames longer than 33ms");
    }

    MFORCEScript::ProfilingData::EnableProfiling(mbProfilerEnabled);
#endif // DEVELOPER
}

void OgreApplication::injectPatsIntoJsContext(const std::string& contextName)
{
#ifdef DEVELOPER

    std::string testScriptPath = PathInfo::Instance()->GetPath(PathInfo::PatsPathID);
    // check if PATS is defined in .pathinfo file, return if not before checking for "PATS_STATE.TXT"
    if (testScriptPath.empty())
        return;

    Poco::Path readWritePath;
    PathInfo::Instance()->GetPath(PathInfo::ReadWritePathID, readWritePath);
    readWritePath.setFileName("PATS_STATE.TXT");
    Poco::File recoveryFile(readWritePath.toString());
    if (recoveryFile.exists())
    {
        testScriptPath = readWritePath.toString();
    }

    if (testScriptPath.empty())
        return;

    Poco::Path path(testScriptPath);
    path.makeAbsolute();
    Poco::File file(path.toString());
    if (file.exists())
    {
        v8::Locker locker;
        v8::HandleScope hs;
        PatsEngine* engine = PatsEngine::getInstance(contextName, true);
        v8::Persistent<v8::Context> context = engine->getContext();
        if (!context.IsEmpty())
        {
            v8::Context::Scope ctxScp(context);
            v8::Local<v8::Script> script = v8::Script::Compile(v8::String::New("var PATS_ScriptEngine = new Pats();"));
            script->Run();
        }

        engine->parse(testScriptPath);
    }
    else
    {
        poco_error(PatsEngine::getLogger(), std::string(__FUNCTION__) + " can not find Platform Automation Test Scriopt (PATS) at: " + path.toString());
    }

#endif // DEVELOPER
}

void OgreApplication::toggleFrameSmoothing()
{
    mbSmoothFrames ^= true;
    if (mpFPSOverlay && mpFPSGraph && !mHibernating)
    {
        if (mbSmoothFrames)
            mpFPSGraph->setTraceColor(2, Ogre::ColourValue::Green);
        else
            mpFPSGraph->setTraceColor(2, Ogre::ColourValue::White);
    }
}

void OgreApplication::toggleFrameTime()
{
    mbFrameTime = !mbFrameTime;
    if (mpFPSGraph && !mHibernating)
    {
        if (mbFrameTime)
        {
            mpFPSGraph->setEntireTrace(0, 33.333f);
            mpFPSGraph->setEntireTrace(1, 16.667f);
        }
        else
        {
            mpFPSGraph->setEntireTrace(0, 60.0f);
            mpFPSGraph->setEntireTrace(1, 30.0f);
        }
    }
}

void OgreApplication::updateFPSDisplay()
{
    float displayValue = mCurrentFPS;
    std::string smoothedText = "";
    if (mbSmoothFrames)
    {
        displayValue = mSmoothedFPS;
        smoothedText = " (smoothed)";
    }
    if (mbFrameTime)
    {
        displayValue = 1.0f / (0.001f * displayValue);
    }
    mpFPSText->setCaption(StringConverter::toString(displayValue, 0) + smoothedText);
    mpFPSGraph->setTraceValue(2, displayValue);
    mpFPSGraph->setTraceValue(3, ((Real)MFORCEScript::ProfilingData::JsFrameTime) / 1000.0f);
}

void OgreApplication::setFrameHitchTrap()
{
    DebugSupportLink& debugLink = DebugSupportLink::Instance();
    debugLink.SetTrapThresholds(mFrameHitchTrapThreshold);
}

void OgreApplication::toggleEqualizerDisplay()
{
    if (mpMasterUI != NULL)
    {
        if (mpMasterUI->IsEqualizerUIActive())
        {
            mpMasterUI->DeactivateEqualizerUI();
        }
        else
        {
            mpMasterUI->ActivateEqualizerUI();
        }
    }
}

void OgreApplication::cycleDelayGameFlowSetting(unsigned int delayMs)
{
    if (delayMs)
    {
        Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
        Ogre::Viewport* pVp = getPrimaryViewport();

        if (mpCycleDelayGameFlowOverlay == NULL)
        {
            mpCycleDelayGameFlowOverlay = om.create("Core/CycleDelayGameFlowOverlay");
            mpCycleDelayGameFlowOverlay->setZOrder(650);

            Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(om.createOverlayElement("Panel", "Core/CycleDelayGameFlowPanel"));
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setPosition(5.0f, 5.0f);
            panel->setDimensions(300.0f, 25.0f);
            panel->setMaterialName("Core/StatsBlockCenter");
            mpCycleDelayGameFlowOverlay->add2D(panel);

            mpCycleDelayGameFlowText = static_cast<Ogre::TextAreaOverlayElement*>(om.createOverlayElement("TextArea", "Core/CycleDelayGameFlowPanelTXT"));
            mpCycleDelayGameFlowText->setMetricsMode(Ogre::GMM_PIXELS);
            mpCycleDelayGameFlowText->setPosition(5.0f, 5.0f);
            mpCycleDelayGameFlowText->setDimensions(300.0f, 25.0f);
            mpCycleDelayGameFlowText->setCharHeight(18.0f);
            mpCycleDelayGameFlowText->setFontName("MessagePopupText");
            mpCycleDelayGameFlowText->setColour(Ogre::ColourValue(1.0f, 0.0f, 0.0f));
            panel->addChild(mpCycleDelayGameFlowText);
        }

        std::ostringstream ss;
        ss << "DELAY GAME FLOW: " << delayMs / 1000.0f << " second";
        mpCycleDelayGameFlowText->setCaption(ss.str());
        CustomizedOverlayManager::getSingletonPtr()->showOverlay(pVp, mpCycleDelayGameFlowOverlay);
    }
    else
    {
        if (mpCycleDelayGameFlowOverlay != NULL)
        {
            Ogre::Viewport* pVp = getPrimaryViewport();
            CustomizedOverlayManager::getSingletonPtr()->hideOverlay(pVp, mpCycleDelayGameFlowOverlay);
            mpCycleDelayGameFlowOverlay->hide();
            Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
            om.destroy(mpCycleDelayGameFlowOverlay);
            om.destroyOverlayElement(mpCycleDelayGameFlowText);
            om.destroyOverlayElement("Core/CycleDelayGameFlowPanel");
            mpCycleDelayGameFlowOverlay = NULL;
            mpCycleDelayGameFlowText = NULL;
        }
    }
}

void OgreApplication::setGameTimeScale(Ogre::Real aScale)
{
    mGameTimeScale.setTimeScale(aScale);
}

Ogre::Real OgreApplication::getGameTimeScale() const
{
    return mGameTimeScale.getTimeScale();
}

void OgreApplication::rampGameTimeScale(Ogre::Real aStart, Ogre::Real aEnd, Ogre::Real aDuration)
{
    mGameTimeScale.rampTimeScale(aStart, aEnd, aDuration);
}

Ogre::Real OgreApplication::getActualFrameTime()
{
    return mActualFrameTime;
}

void OgreApplication::videoMemFragCheck(long long deltaMicroseconds)
{
    // fragmented video memory typically manifests with the pattern of alternating slow and fast frames:
    // From game on radisys, when it was thrashing:
    // frame times in (ms)
    // :  15  29   9  12  15  28 107   9 101   6  83   7  81   7  80 
    // :  29   9  12  15  28 107   9 101   6  83   7  81   7  80  11 
    // :   9  12  15  28 107   9 101   6  83   7  81   7  80  11  83  
    // :  12  15  28 107   9 101   6  83   7  81   7  80  11  83   8 
    // :  99  99  83   6  99 110  99   6 110 100  81 113  89  76   5  
    // :  76  90  75  67  90  83  66   6  99  83  89  70  93  83   9 

    mLastDeltasToTrack[mLastDeltasToTrackIdx] = deltaMicroseconds;
    mLastDeltasToTrackIdx = (mLastDeltasToTrackIdx + 1) % FRAMES_TO_TRACK;
    int slowCount = 0;
    int fastCount = 0;
    long long totalTime = 0;
    for (int i = 0; i < FRAMES_TO_TRACK; ++i)
    {
        long long delta = mLastDeltasToTrack[i] / 1000ULL;
        if (delta)
        {
            if (delta < 11)  // 11 ms ~= 90 fps
            {
                fastCount++;
            }
            else if (delta > 66) // 66ms ~= 15 fps
            {
                slowCount++;
            }
            totalTime += delta;
        }
    }
    bool frag = false;
    if (totalTime < 800)
    {
        // fragmentation looks like a bunch of fast and slow frames in a small window of time.
        if ((fastCount >= FAST_FRAME_CNT) && (slowCount >= SLOW_FRAME_CNT))
            frag = true;
    }
    else if (totalTime > 950)
    {
        // or fragmentation looks like its running faster than 60 ???
        if (totalTime > 1020) // 15 frame average > ~68 fps
        {
            if ((fastCount > 4) && (slowCount > 2))
                frag = true;
        }
        else if ((fastCount + slowCount) >= (FAST_FRAME_CNT + SLOW_FRAME_CNT))
            frag = true;
    }

    if (frag)
    {
        // throttle evict/log calls to once per 3 minutes
        unsigned long long currentTime = MicrosecondsSinceStart();
        if ((currentTime - mLastEvictTime) > 3ULL * 60ULL * 1000ULL * 1000ULL)
        {
            char buf[16] = { 0 };
            char line[256];
            strcpy(line, "Possible vram fragmentation detected: ");
            for (int i = 0; i < FRAMES_TO_TRACK; ++i)
            {
                int idx = (mLastDeltasToTrackIdx + i) % FRAMES_TO_TRACK;
                sprintf(buf, "%3d ", (int)(mLastDeltasToTrack[idx]) / 1000);
                strcat(line, buf);
            }
            poco_information(Poco::Logger::get(MODULENAME_GAMEENGINE), line);

            evictManagedResources();
            mEvictCount++;
            mLastEvictTime = currentTime;
        }
    }
}

std::string OgreApplication::getTrackedFps()
{
    char buf[16] = { 0 };
    char line[256] = { 0 };
    for (int i = 0; i < FRAMES_TO_TRACK; ++i)
    {
        int idx = (mLastDeltasToTrackIdx + i) % FRAMES_TO_TRACK;
        int delta = static_cast<int>(mLastDeltasToTrack[idx]);
        if (delta)
        {
            sprintf(buf, "%2d ", (int)(1000000.0 / delta));
            strcat(line, buf);
        }
    }
    return std::string(line);
}

long long OgreApplication::smoothFrame(long long deltaMicroseconds)
{
    long long currentDelta = deltaMicroseconds;

    // don't try to smooth large durations. it won't help anyway.
    if (deltaMicroseconds < 100000)
    {
        // calculate the average of the last x frames, and use it as the duration
        long long tot = 0;
        for (int i = 0; i < FRAMES_TO_AVG; ++i)
            tot += mLastDeltasToAvg[i];

        currentDelta = (int)(tot / FRAMES_TO_AVG);
        mLastDeltasToAvg[mLastDeltasToAvgIdx] = deltaMicroseconds;
        mLastDeltasToAvgIdx = (mLastDeltasToAvgIdx + 1) % FRAMES_TO_AVG;

        // if the frame rate avg is low (slower than 45 fps), don't bother trying
        if (currentDelta > (1000000 / 45))
        {
            currentDelta = deltaMicroseconds;
        }
        else
        {
            if (deltaMicroseconds > currentDelta)
            {
                // a slow frame - report shorter time, but track time difference  so we can account for it back in.
                mDeltaCarryover += deltaMicroseconds - currentDelta;
            }
            else
            {
                // subtract off the difference between average time and elapsed time
                mDeltaCarryover -= (currentDelta - deltaMicroseconds);

                // if there's still time in the time bank, add it back it too
                if (mDeltaCarryover > 0)
                {
                    long long amtToAdj = 2000;
                    if (mDeltaCarryover <= 2000)
                        amtToAdj = mDeltaCarryover;
                    currentDelta += amtToAdj;
                    mDeltaCarryover -= amtToAdj;
                }
                else
                    mDeltaCarryover = 0;
            }
        }
    }
    Ogre::Real smoothed = currentDelta * 0.000001f;
    mSmoothedFPS = floor((1.0f / smoothed) + 0.5f);
    return currentDelta;
}

void OgreApplication::TrackFrameRates(bool fEnable)
{
    if (mTrackFrameRates != fEnable)
    {
        mTrackFrameRates = fEnable;
#ifdef WIN32
        if (mRenderLoopSharedCounter != NULL)
        {
            SetSharedCounterProperties(mRenderLoopSharedCounter, (fEnable) ? (SC_PROP_DEFAULT) : (SC_PROP_COUNTER_DISABLED));
        }
#endif
    }
}

void OgreApplication::enterReplayMode(const Json::Value& aPlayHistory)
{
    if (mpMasterUI && !mpMasterUI->IsReplayUIActive())
    {
        mpMasterUI->ActivateReplayUI(aPlayHistory);
        NotificationManager::Instance().PostNotification(NotificationPtr(new EnterReplayModeNotification()));
    }
}

void OgreApplication::exitReplayMode()
{
    if (mpMasterUI && mpMasterUI->IsReplayUIActive())
    {
        mpMasterUI->DeactivateReplayUI();
        NotificationManager::Instance().PostNotification(NotificationPtr(new ExitReplayModeNotification()));
    }
}

bool OgreApplication::isInReplayMode() const
{
    return(mReplayRecallModeActive || mReplayRestoreModeActive);
}

bool OgreApplication::isInReplayRecall() const
{
    return(mReplayRecallModeActive);
}

bool OgreApplication::isInReplayRestore() const
{
    return(mReplayRestoreModeActive);
}

bool OgreApplication::isInEvaluationMode() const
{
    return(mEvaluationModeActive);
}

bool OgreApplication::gameControlsOdometersInRecall() const
{
    return DataLayer::Repository::Instance().GetGamePlayDataSource()->GetGameControlsOdometersInRecall(GetActiveThemeId());
}

void OgreApplication::showMessageDialog(const std::string& title, const std::string& message)
{
    if (mpMasterUI && !mpMasterUI->IsMessageDialogActive())
    {
        mpMasterUI->ShowMessageDialog(title, message);
    }
}

void OgreApplication::hideMessageDialog()
{
    if (mpMasterUI && mpMasterUI->IsMessageDialogActive())
    {
        mpMasterUI->HideMessageDialog();
    }
}

void OgreApplication::IncreaseRestoreUIOverlayAlpha()
{
    if (mpMasterUI)
    {
        mpMasterUI->IncreaseRestoreUIOverlayAlpha();
    }
}

void OgreApplication::DecreaseRestoreUIOverlayAlpha()
{
    if (mpMasterUI)
    {
        mpMasterUI->DecreaseRestoreUIOverlayAlpha();
    }
}

void OgreApplication::EnableReplayNextButton(bool enableFlag)
{
    if (mpMasterUI)
    {
        if (mpMasterUI->IsReplayUIActive())
        {
            mpMasterUI->EnableReplayNextButton(enableFlag);
        }
    }
}

void OgreApplication::EnableReplayPrevButton(bool enableFlag)
{
    if (mpMasterUI)
    {
        if (mpMasterUI->IsReplayUIActive())
        {
            mpMasterUI->EnableReplayPrevButton(enableFlag);
        }
    }
}

void OgreApplication::EnableReplayQuickStopButton(bool enableFlag)
{
    if (mpMasterUI)
    {
        if (mpMasterUI->IsReplayUIActive())
        {
            mpMasterUI->EnableReplayQuickStopButton(enableFlag);
        }
    }
}

void OgreApplication::onReplayEvent(const Json::Value& aMsg)
{
    static eUpdateSpeedMode savedUpdateSpeedMode = UpdateNormal;
    static int savedUpdateSpeed = 1;

    const std::string& msgFunction = aMsg.get("msgFunction", "").asString();

    // For Unity applications, set a separate variable and exit early.
    // This is so that OgreApplication will know not to open the Demo Menu during Restore
    // when Bonus Gaffing is enabled, but will not try to invoke Ogre-specific replay code
    // that shouldn't run for Unity games.
    if (!mpMasterUI)
    {
        if (msgFunction == "GameEngineEnterReplayMode" || msgFunction == "EnterReplayRestoreMode")
        {
            mUnityReplayModeActive = true;
        }
        else if (msgFunction == "ExitReplayMode" || msgFunction == "ExitReplayRestoreMode")
        {
            mUnityReplayModeActive = false;
        }

        return;
    }

    //NOTE: we will not handle the msgFunction "EnterReplayMode", instead we wait for the state manager to send
    //a "GameEngineEnterReplayMode" msgFunction
    if (msgFunction == "GameEngineEnterReplayMode")
    {
        Json::Value playHistory = aMsg.get("gamePlayRecord", "");
        enterReplayMode(playHistory);
        mReplayRecallModeActive = true;

        //save off the current speed mode and speed (we will not change it unless we get an explicit message to do so)
        savedUpdateSpeedMode = mUpdateSpeedMode;
        savedUpdateSpeed = mUpdateSpeed;
    }
    else if (msgFunction == "ExitReplayMode")
    {
        exitReplayMode();
        mReplayRecallModeActive = false;

        //reset the speed mode and speed to the saved values
        mUpdateSpeedMode = savedUpdateSpeedMode;
        mUpdateSpeed = savedUpdateSpeed;
    }
    else if (msgFunction == "ReplayModeSetup")
    {
        bool setup = aMsg.get("SetupFlag", true).asBool();
        if (setup)
        {
            mpMasterUI->ReplaySetup();
        }
        else
        {
            mpMasterUI->ReplaySetupCompleted();
        }
    }
    else if (msgFunction == "ReplayModeSpeed")
    {
        if (mReplayRecallModeActive)
        {
            unsigned int speedup = aMsg.get("Speedup", 1).asUInt(); //default of 1 for replay mode speedup

            //range for speedup limited to 1...8
            if (speedup < 1) speedup = 1;
            else if (speedup > 8) speedup = 8;
            if (speedup > 1)
            {
                mUpdateSpeedMode = UpdateFast;//= UpdateFast,UpdateNormal 
                mUpdateSpeed = speedup;
            }
            else
            {
                mUpdateSpeedMode = UpdateNormal;//= UpdateFast,UpdateNormal 
                mUpdateSpeed = speedup;
            }
        }
    }
    else if (msgFunction == "EnterReplayRestoreMode")
    {
        mReplayRestoreModeActive = true;
        mpMasterUI->ActivateRestoreUI();
        NotificationManager::Instance().PostNotification(NotificationPtr(new EnterReplayModeNotification()));

        //save off the current speed mode and speed
        savedUpdateSpeedMode = mUpdateSpeedMode;
        savedUpdateSpeed = mUpdateSpeed;

        unsigned int restoreSpeedup = aMsg.get("restoreSpeedup", 8).asUInt(); //default of 8 for restoreSpeedup

        //range for restoreSpeedup limited to 1...8
        if (restoreSpeedup < 1) restoreSpeedup = 1;
        else if (restoreSpeedup > 8) restoreSpeedup = 8;

        //set the speed mode to fast and the speed to restoreSpeedup
        mUpdateSpeedMode = UpdateFast;//= UpdateFast,UpdateNormal 
        mUpdateSpeed = restoreSpeedup;

    }
    else if (msgFunction == "ExitReplayRestoreMode")
    {
        mpMasterUI->DeactivateRestoreUI();
        NotificationManager::Instance().PostNotification(NotificationPtr(new ExitReplayModeNotification()));
        mReplayRestoreModeActive = false;

        //reset the speed mode and speed to the saved values
        mUpdateSpeedMode = savedUpdateSpeedMode;
        mUpdateSpeed = savedUpdateSpeed;
    }
    else if (msgFunction == "NextBonusRequestPayload")
    {
        Json::Value payload = aMsg.get("payload", Json::nullValue);
        if (payload != Json::nullValue)
            NotificationManager::Instance().PostNotification(NotificationPtr(new ReplayBonusPayloadNotification(payload)));
    }
    else if (msgFunction == "EnableNextButton")
    {
        bool enableFlag = aMsg.get("enableFlag", false).asBool();
        EnableReplayNextButton(enableFlag);
    }
    else if (msgFunction == "EnablePrevButton")
    {
        bool enableFlag = aMsg.get("enableFlag", false).asBool();
        EnableReplayPrevButton(enableFlag);
    }
    else if (msgFunction == "EnableQuickStopButton")
    {
        bool enableFlag = aMsg.get("enableFlag", false).asBool();
        EnableReplayQuickStopButton(enableFlag);
    }
    else if (msgFunction == "RestoreProgressPercent")
    {
        int percentComplete = aMsg.get("percentComplete", 0).asInt();
        mpMasterUI->SetRestoreProgress(percentComplete);
    }
    else if (msgFunction == "RestoreProgressTargetPercent")
    {
        int percentComplete = aMsg.get("percentComplete", 0).asInt();
        mpMasterUI->SetRestoreTargetProgress(percentComplete);
    }
}

void OgreApplication::onEvaluationModeEvent(const Json::Value& aMsg)
{
    const std::string& msgFunction = aMsg.get("msgFunction", "").asString();
    if (msgFunction == "EnterEvaluationMode")
    {
        mEvaluationModeActive = true;

        // Early exit if Master UI has not been initialized - temporary use for Unity display of Demo Menu in Eval mode.
        if (!mpMasterUI)
        {
            return; 
        }

        mpMasterUI->ActivatePrizeEvaluationUI();
        setTargetFPS(mDefaultFPS);
    }
    else if (msgFunction == "ExitEvaluationMode")
    {
        mEvaluationModeActive = false;

        // Early exit if Master UI has not been initialized - temporary use for Unity display of Demo Menu in Eval mode.
        if (!mpMasterUI)
        {
            return;
        }

        mpMasterUI->DeactivatePrizeEvaluationUI();
        if (IsDemoMenuActive())
        {
            //close demo menu if it was opened in evaluation mode
            DeactivateDemoMenu();
        }
        if (IsGaffingUIActive())
        {
            mpMasterUI->DeactivateGaffingUI();
        }
        mGameSpecificData.clear();
        setTargetFPS(mTechUIFPS);
    }
    else if (msgFunction == "ConfigureEvaluationMode")
    {
        // Early exit if Master UI has not been initialized - temporary use for Unity display of Demo Menu in Eval mode.
        if (!mpMasterUI)
        {
            return;
        }
        mpMasterUI->ConfigurePrizeEvaluationUI(aMsg);
    }
}


#if DEVELOPER
void MFORCE::GameEngine::ReelTimingEvent::Start(const void * /*sender*/, int&)
{
    reelSpinStart = MicrosecondsSinceStart();
}

void MFORCE::GameEngine::ReelTimingEvent::Stop(const void * /*sender*/, int&)
{
    reelSpinStop = MicrosecondsSinceStart();
    if (userMessageController)
    {
        if (ogreApplication->mEnableReelTiming)
        {
            userMessageController->setEGMMessage("Total spin time: " + StringConverter::toString((reelSpinStop - reelSpinStart) / 1000000.0f));
        }
        else
        {
            userMessageController->setEGMMessage("");
        }
    }
}

MemoryRenderBuffer* OgreApplication::GetSnapshotMemoryRenderBufferForScreen(const std::string& windowName)
{
    for (auto mrb : mMemoryRenderBuffers)
    {
        if (mrb->GetUsage() == Usage::Screenshot && mrb->SrcWindowName()==windowName)
        {
            return mrb;
        }
    }
    return nullptr;
}

#endif // DEVELOPER


void OgreApplication::loadExtraResources(const std::string& packageId, const std::string& groupName)
{
    // Prevent rendering while we are manipulating resources
    OGRE_NAMED_MUTEX_SCOPED_LOCK

    CustomizedOverlayManager::getSingleton().clearLoadedScripts();

    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
    if (pAtlasPlugin)
    {
        if (pAtlasPlugin->getTextureAtlas(packageId).isNull())
        {
            TextureAtlasPtr pTextureAtlas = pAtlasPlugin->createTextureAtlas(packageId);
            pTextureAtlas->setTextureGroupName(packageId + "Atlas");
            pTextureAtlas->initialise();
            pAtlasPlugin->setCurrentTextureAtlas(packageId);
        }
        else
        {
            poco_warning(Poco::Logger::get("gameengine"), "Texture Atlas for " + packageId + " already exists.");
        }
    }
    else
    {
        poco_error(Poco::Logger::get("gameengine"), "Can not find 'AtlasWidgetPlugin'. A Texture Atlas for " + packageId + " can not be created.");
    }

    // Is this is an MGUI package, then make it an owner of some components
    if (isMGUIPackage(packageId))
    {
        linkComponentToThemeId("MarqueeEmulator", packageId);
    }

    DataLayer::ThemeComponentList componentList = GetSystemComponents(packageId);
    if (!OgreResources::Instance()->LoadResources(packageId, componentList, groupName))
    {
        // Resources have already been loaded for this group, notify the appropriate plugin
        std::string pluginName = getPluginNameByPackageName(groupName);
        PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(pluginName));

        if (pPackageManagerPlugin)
        {
            pPackageManagerPlugin->onResourcesLoaded(packageId);
        }
        else
        {
            poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::loadExtraResources() - couldn't find PackageManager plugin associated with packageId: " + packageId);
        }
    }
}

void OgreApplication::sendLoadPackageAudio(const std::string& packageId, const std::string& packagePath)
{
    Json::Value evt;
    Message::InitializeMessage(GROUP_SYSTEM, "LoadPackageAudioEvent", Message::EVENT, evt);

    evt["packageId"] = packageId;
    evt["packagePath"] = packagePath;

    SendEvent(evt);
}

void OgreApplication::sendUnloadPackageAudio(const std::string& packageId)
{
    Json::Value evt;
    Message::InitializeMessage(GROUP_SYSTEM, "UnloadPackageAudioEvent", Message::EVENT, evt);

    evt["packageId"] = packageId;

    SendEvent(evt);
}

bool OgreApplication::ResourceExists(const Poco::Path& rootPath, const std::string& filePattern) const
{
    return OgreResources::Instance()->GetResourceCatalog().ResourceExists(rootPath, filePattern);
}

void OgreApplication::ExtraResourcesLoaded(const std::string& resourceGroupName)
{
    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
    if (pAtlasPlugin)
    {
        TextureAtlasPtr pTextureAtlas = pAtlasPlugin->getTextureAtlas(resourceGroupName);
        pTextureAtlas->prepareAllTextures();
    }

    // force all of the overlays to initialize
    Ogre::OverlayManager::OverlayMapIterator elementIt = Ogre::OverlayManager::getSingleton().getOverlayIterator();
    while (elementIt.hasMoreElements())
    {
        Overlay* pOverlay = elementIt.getNext();
        pOverlay->show();
        pOverlay->hide();
    }

    mRoot->renderOneFrame(0.0f);

    std::string pluginName = getPluginNameByPackageName(resourceGroupName);
    PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(pluginName));

    if (pPackageManagerPlugin)
    {
        //MPRTODO: resourceGroupName is the context name?
        pPackageManagerPlugin->onResourcesLoaded(resourceGroupName);
        if (isMGUIPackage(resourceGroupName))
        {
            mLoadDependencyMgr.setDependencyLoaded(resourceGroupName, true);
        }
        else if (isSNGUIPackage(resourceGroupName))
        {
            mLoadDependencyMgr.setDependencyLoaded(resourceGroupName, true);
        }

    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::ExtraResourcesLoaded() - couldn't find PackageManager plugin associated with resource group: " + resourceGroupName);
    }

    const std::string themeId = ExtractThemeId(resourceGroupName);

    if (mLoadDependencyMgr.isRegisteredDependency(themeId))
    {
        mLoadDependencyMgr.setDependencyLoaded(themeId, true);

        // If the game engine was awaiting authentication to load a package, onLoadComplete() must be called
        // once all dependencies are loaded.
        if (mLoadDependencyMgr.allDependenciesLoaded())
        {
            onLoadComplete();
        }
    }
}

void OgreApplication::preloadCompleteCommon()
{
    // force all of the overlays to initialize
    Ogre::OverlayManager::OverlayMapIterator elementIt = Ogre::OverlayManager::getSingleton().getOverlayIterator();
    while (elementIt.hasMoreElements())
    {
        Overlay* pOverlay = elementIt.getNext();
        pOverlay->show();
        pOverlay->hide();
    }
    // make sure the next call to process won't skip a frame.
    mLastUpdateTime = 0;

    mLoadStatus = LOAD_COMPLETE;
    mRoot->clearEventTimes();
    
    CustomizedOverlayManager::getSingletonPtr()->enableOverlaySorting(true);
    CustomizedOverlayManager::getSingletonPtr()->updateOverlayOrder();
}

void OgreApplication::onBundleLoadComplete()
{
    preloadCompleteCommon();

    // notify the rest of the system
    Json::Value dataLoaded;
    Message::InitializeMessage(GROUP_SYSTEM, "BundleDataLoadedNotification", Message::EVENT, dataLoaded);
    dataLoaded["bundleId"] = mActiveBundle;
    dataLoaded["moduleName"] = MODULENAME_GAMEENGINE;
    dataLoaded["success"] = true;
    SendEvent(dataLoaded);
    mpOgreLog->logMessage(String("Bundle Loaded: " + MultiGameManager::getInstance().getBundleName(mActiveBundle)));

    poco_notice(Poco::Logger::get("gameengine"), Poco::format("Bundle %s loaded and ready to start.", MultiGameManager::getInstance().getBundleName(mActiveBundle)));
}

void OgreApplication::onPreloadThemeComplete()
{
    preloadCompleteCommon();

    // notify the rest of the system
    Json::Value gamePreloaded;
    Message::InitializeMessage(GROUP_SYSTEM, "GameDataPreloadedNotification", Message::EVENT, gamePreloaded);
    gamePreloaded["moduleName"] = MODULENAME_GAMEENGINE;
    gamePreloaded["success"] = true;
    SendEvent(gamePreloaded);
    mpOgreLog->logMessage(String("Game preloaded"));
}

void OgreApplication::unloadExtraResources(const std::string& packageId, const std::string& overlayElementPrefix)
{
    // Prevent rendering while we are manipulating resources
    OGRE_NAMED_MUTEX_SCOPED_LOCK

        poco_debug(Poco::Logger::get("gameengine"), "Freeing " + packageId + " resources ...");
    AtlasWidgetPlugin* pAtlasPlugin = (AtlasWidgetPlugin*)(getPluginByName("Plugin_AtlasWidget"));
    if (pAtlasPlugin)
    {
        TextureAtlasPtr pTextureAtlas = pAtlasPlugin->getTextureAtlas(packageId);

        if (pTextureAtlas.isNull() == false)
        {
            pTextureAtlas->shutDown();
        }
        else
        {
            poco_warning(Poco::Logger::get("gameengine"), "Texture Atlas with name " + packageId + " does not exist, so skipping shutDown call to it.");
        }

        pAtlasPlugin->PurgeInstancesByGroup(packageId);
        pAtlasPlugin->destroyTextureAtlas(packageId);
        pAtlasPlugin->setCurrentTextureAtlas("Game");
    }
    else
    {
        poco_warning(Poco::Logger::get("gameengine"), "Atlas Widget Plugin could not be found. Skipping tear down of " + packageId + " atlas items.");
    }

    CustomizedOverlayManager::getSingleton().destroyOverlayElementsWithPrefix(overlayElementPrefix.c_str(), false);
    CustomizedOverlayManager::getSingleton().destroyOverlayElementsWithPrefix(overlayElementPrefix.c_str(), true);

    const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
    Ogre::Root::PluginInstanceList::const_iterator iter;
    for (iter = plugins.begin(); iter != plugins.end(); ++iter)
    {
        ScriptablePlugin* pPlugin = dynamic_cast<ScriptablePlugin*>(*iter);
        if (pPlugin)
        {
            pPlugin->PurgeInstancesByGroup(packageId);
        }
    }

    OgreResources::Instance()->PurgeResources(packageId);

    delinkComponentsOfThemeId(packageId);
}

void OgreApplication::ExtraResourcesUnloaded(const std::string& resourceGroupName)
{
    const std::string packageId = ExtractThemeId(resourceGroupName);
    if (mLoadDependencyMgr.isRegisteredDependency(packageId))
    {
        mLoadDependencyMgr.setDependencyLoaded(packageId, false);
        mLoadDependencyMgr.unregisterDependency(packageId);
    }

    std::string pluginName = getPluginNameByPackageName(resourceGroupName);
    PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(pluginName));

    if (pPackageManagerPlugin)
    {
        pPackageManagerPlugin->onResourcesUnloaded(resourceGroupName);
        if (isMGUIPackage(resourceGroupName))
        {
            mLoadDependencyMgr.setDependencyLoaded(resourceGroupName, false);
            mLoadDependencyMgr.unregisterDependency(resourceGroupName);
            onBundleUnloadComplete();
        }
        else if (isSNGUIPackage(resourceGroupName))
        {
            mLoadDependencyMgr.setDependencyLoaded(resourceGroupName, false);
            mLoadDependencyMgr.unregisterDependency(resourceGroupName);
        }

    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::ExtraResourcesUnloaded() - couldn't find PackageManager plugin associated with resource group: " + resourceGroupName);
    }
}

void OgreApplication::onBundleUnloadComplete()
{
    // notify the rest of the system
    Json::Value dataUnloaded;
    Message::InitializeMessage(GROUP_SYSTEM, "BundleDataUnloadedNotification", Message::EVENT, dataUnloaded);
    dataUnloaded["bundleId"] = mActiveBundle;
    dataUnloaded["moduleName"] = MODULENAME_GAMEENGINE;
    dataUnloaded["success"] = true;
    SendEvent(dataUnloaded);
    mpOgreLog->logMessage(String("Bundle unloaded: " + MultiGameManager::getInstance().getBundleName(mActiveBundle)));
    poco_notice(Poco::Logger::get("gameengine"), Poco::format("Bundle %s unloaded successfully.", MultiGameManager::getInstance().getBundleName(mActiveBundle)));

    // Remove the components of this bundle and the MGUI package
    delinkComponentsOfThemeId(mActiveBundle);

    mActiveBundle.clear();
}

void OgreApplication::onPackageRequest(const Json::Value& msg)
{
    const std::string msgTypeName = msg["typeName"].asString();

    std::string packageManagerPluginName;

    //MPRTODO: set something on outbound msg?
    if (msgTypeName == "DLCPackageRequest_GameEngine")
    {
        packageManagerPluginName = PackageManagerPluginName::DLCPackage;
    }
    else if (msgTypeName == "MultiGameUIPackageRequest")
    {
        packageManagerPluginName = PackageManagerPluginName::MultiGamePackage;
        MFORCE::PackageManagerRequestType requestType = static_cast<MFORCE::PackageManagerRequestType>(msg.get("requestType", eInvalidPackageManagerRequestType).asUInt());
        if (requestType == ePackageRequestType_Execute)
        {
            Json::Value command;
            bool bParseOk = Json::Reader().parse(msg.get("command", "").asCString(), command);
            poco_assert(bParseOk);
            const std::string& themeId = command.get("themeId", "").asString();
            mMGUIMaximized = command.get("maximize", false).asBool();
            if (mMGUIMaximized)
            {
                EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
                egmPlugin->StopGameMarqueeScripts();
                mpIdleScene->ShowLoadingScene(false);
            }
            else
            {
                mpIdleScene->SetActiveThemeId(themeId);
                mpIdleScene->ShowLoadingScene(!themeId.empty());
            }
            mpIdleScene->ShowLoadingOverlays(false);
            mpIdleScene->ForceUpdateDrawLoading();
            mpMasterUI->DisplayUnavailableScreens(false);
        }
        else if (requestType == ePackageRequestType_Stop)
        {
            const std::string& themeId = msg.get("themeId", "").asString();
            mpIdleScene->ShowLoadingOverlays(false);
            mpIdleScene->ShowLoadingScene(!themeId.empty());
            mpIdleScene->SetActiveThemeId(themeId);
            mpIdleScene->ForceUpdateDrawLoading();
            mpMasterUI->DisplayUnavailableScreens(false);
        }
    }
    else if (msgTypeName == "SitAndGoUIPackageRequest")
    {
        packageManagerPluginName = PackageManagerPluginName::SitAndGoUIPackage;
    }

    PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(packageManagerPluginName));
    if (pPackageManagerPlugin)
    {
        pPackageManagerPlugin->handlePackageRequest(msg);
    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), Poco::format("Could not find PackageManager '%s' when handling package request:\t%s", packageManagerPluginName, msg.toStyledString()));
    }
}

void OgreApplication::onUnityPackageRequest(const Json::Value& msg)
{
    const std::string msgTypeName = msg["typeName"].asString();
    if (msgTypeName == "MultiGameUIPackageRequest")
    {
        const std::string packageID = "MGUI.MultigameUI";
        const std::string& packagePath = msg.get("extracted_path", "").asString();
        MFORCE::PackageManagerRequestType requestType = static_cast<MFORCE::PackageManagerRequestType>(msg.get("requestType", eInvalidPackageManagerRequestType).asUInt());

        // TODO: GRHDEV-23675 Unity MGUI resources load/unload and clear unnecessary code
        // Currently only audio is loaded/unloaded through the way OGRE MGUI audio load, other resources are not loaded/unloaded.
        if (requestType == ePackageRequestType_Load)
        {
            const std::string rootDriveLetter(1, packagePath[0]);
            PathInfo::Instance()->AddPath(Poco::Path(rootDriveLetter), "Game_" + packageID, packagePath, PathInfo::volumeDLC);
            PathInfo::Instance()->AddGamePath(packageID, packagePath);

            sendLoadPackageAudio(packageID, packagePath);
            //loadExtraResources(packageID, ExtraResourcesGroupName);

            Json::Value pkgMsg;
            MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM, "PackageDataLoaded", MFORCE::Messaging::Message::EVENT, pkgMsg);
            pkgMsg["packageId"] = packageID;
            pkgMsg["moduleName"] = MODULENAME_GAMEENGINE;
            pkgMsg["success"] = true;
            SendEvent(pkgMsg);
        }
        else if (requestType == ePackageRequestType_Unload)
        {
            sendUnloadPackageAudio(packageID);
            //unloadExtraResources(packageID, packageID + ".");

            Json::Value pkgMsg;
            MFORCE::Messaging::Message::InitializeMessage(GROUP_SYSTEM, "PackageDataUnloaded", MFORCE::Messaging::Message::EVENT, pkgMsg);
            pkgMsg["packageId"] = packageID;
            pkgMsg["moduleName"] = MODULENAME_GAMEENGINE;
            SendEvent(pkgMsg);
        }
    }
}

void OgreApplication::onSitAndGoUIPreloadEvent(const Json::Value& msg)
{
    const std::string msgTypeName = msg["typeName"].asString();

    const std::string& packagePath = msg.get("extracted_path", "").asString();
    mLoadDependencyMgr.registerDependency("SNGUI.SitAndGoUI");
    mPreloadManager.preloadPackage("SNGUI.SitAndGoUI", packagePath);

}


void OgreApplication::sendEnableTouchEvent(const std::string& windowName, bool enable)
{
    Json::Value enableTouch;
    Message::InitializeMessage(GROUP_IOINPUT, "EnableTouch", Message::EVENT, enableTouch);
    enableTouch[Json::StaticString("enable")] = enable;
    enableTouch[Json::StaticString("monitorName")] = windowName;
    SendEvent(enableTouch);
}

void OgreApplication::onNitroBonusAuthenticated(const Json::Value& msg)
{
    const std::string& themeId = msg.get("themeId", "").asString();
    const std::string& context = "DLC." + msg.get("context", "").asString();
    const std::string& path = msg.get("path", "").asString();

    mContextNameToThemeIdMap[context] = themeId;
    UnifiedMessageBroadcast::instance().setContextNameToThemeIdMap(mContextNameToThemeIdMap);

    for (auto& bonusData : mActiveGame.NitroBonusData)
    {
        if (bonusData.get("themeId", "").asString() == themeId)
        {
            bonusData["path"] = path;
        }
    }

    Json::Value loadMsg;
    Message::InitializeMessage(GROUP_SYSTEM, "SuggestLoadPackage", Message::EVENT, loadMsg);
    loadMsg[Json::StaticString("pluginRequestType")] = "DLCPackageRequest_GameEngine";
    loadMsg[Json::StaticString("packagePath")] = path;
    SendEvent(loadMsg);
}

void OgreApplication::activateBlockerOverlays( /*const BlockerOverlays& blockers,*/ const bool show)
{
    // For each window, put up an overlay if it requires one.
    for (const auto& it : mWindows)
    {
        Ogre::RenderWindow* rw = it.second;
        const auto& wname = rw->getName();

        if (("MECH_REELS" != wname) && ("MARQUEE" != wname))
        {
            Ogre::Overlay* blocker = nullptr;

            // Lookup the blocker overlay based on window name.
            const auto& lookup = mBlockerOverlays.find(wname);
            if (lookup != mBlockerOverlays.end())
            {
                blocker = lookup->second;

                auto vp = getTopViewport(wname);
                if (show)
                {
                    CustomizedOverlayManager::getSingleton().showOverlay(vp, blocker, true);
                }
                else
                {
                    CustomizedOverlayManager::getSingleton().hideOverlay(vp, blocker);
                }
            }
        }
    }
}

static std::string getBlockerOverlayName(std::string aWindowName)
{
    std::string overlayName = "Core/BoltOnBonusBlocker" + aWindowName;
    return overlayName;
}

static std::string getBlockerContainerName(std::string aWindowName)
{
    std::string containerName = "Core/BoltOnBonusBlockerContainer" + aWindowName;
    return containerName;
}

static std::string getBlockerSpriteName(std::string aWindowName)
{
    std::string spriteName = "Core/BoltOnBonusBlockerSprite" + aWindowName;
    return spriteName;
}


Ogre::Overlay* OgreApplication::createBlockerOverlay(std::string aWindowName)
{
    Ogre::OverlayManager& overlayMgr = Ogre::OverlayManager::getSingleton();

    // create the overlay
    std::string overlayName = getBlockerOverlayName(aWindowName);
    Ogre::Overlay* blockerOverlay = overlayMgr.create(overlayName);
    blockerOverlay->setZOrder(499); // primary game overlays are assumed to have z-order < 500 while Nitro bonus overlays have z-orders >= 500 

    // create the container
    std::string containerName = getBlockerContainerName(aWindowName);
    SpriteOverlayElement* blockerContainer = dynamic_cast<SpriteOverlayElement*>(overlayMgr.createOverlayElement("Sprite", containerName));
    blockerOverlay->add2D(blockerContainer);

    // create the sprite
    std::string spriteName = getBlockerSpriteName(aWindowName);
    SpriteOverlayElement* blockerSprite = dynamic_cast<SpriteOverlayElement*>(overlayMgr.createOverlayElement("Sprite", spriteName));
    blockerContainer->addChild(blockerSprite);

    Ogre::ColourValue blockerColor(0, 0, 0, 1);
    blockerSprite->setColor(blockerColor);
    blockerSprite->setIgnoresMouse(false);
    blockerSprite->setMaterialName("Core/ReplayBackground");

    return blockerOverlay;
}

void OgreApplication::setNitroBonusActive(const bool active, const std::string& contextName)
{
    std::string themeId;
    if (active)
    {
        bool bIsMappedToTheme = ConvertContextNameToThemeId(contextName, themeId);
        poco_assert(bIsMappedToTheme && !themeId.empty());
    }
    else
    {
        // go back to primary theme..
        themeId = mActiveGame.ThemeId;
    }

    mpRenderQueueController->setActiveTheme(themeId);

    // Put up (or remove) blocker overlays to prevent clicks from falling through to the primary game.
    OgreApplication* ogreApp = OgreApplication::getSingletonPtr();
    const OgreApplication::WindowMap& wm = ogreApp->getRenderWindowMap();
    OgreApplication::WindowMap::const_iterator iter;
    for (iter = wm.begin(); iter != wm.end(); ++iter)
    {
        RenderWindow* rw = (*iter).second;
        const Ogre::String &windowName = rw->getName();
        if (("MAIN" == windowName) || ("MINI" == windowName) || ("BUTTON_PANEL" == windowName))
        {
            if (!mBlockerOverlays.count(windowName))
            {
                // create the blocker overlay and hold on to it
                mBlockerOverlays[windowName] = createBlockerOverlay(windowName);

            }


        }
    }

    // Put up (or remove) blocker overlays to prevent clicks from falling through to the primary game.

    activateBlockerOverlays(active);

    mIsNitroBonusActive = active;
}

bool OgreApplication::isNitroBonusTheme(const std::string& themeId) const
{
    return mNitroBonusThemes.find(themeId) != mNitroBonusThemes.end();
}

// Note that the following method requires name to have one of the following formats:
//  <>.<ThemeId> or
//  <>.<ThemeId>.<AssetName>
std::string OgreApplication::ExtractThemeId(const std::string& name)
{
    size_t firstPeriodPos = name.find_first_of('.');
    if (firstPeriodPos == std::string::npos)
    {
        return name;
    }
    else
    {
        size_t secondPeriodPos = name.find_first_of('.', (firstPeriodPos + 1));

        if (secondPeriodPos == std::string::npos)
        {
            // No second period, so return everything after the first period.
            return name.substr(firstPeriodPos + 1);
        }
        else
        {
            // Return what's between the two periods.
            return name.substr(firstPeriodPos + 1, secondPeriodPos - firstPeriodPos - 1);
        }
    }
}

void OgreApplication::SaveWindowResolutions()
{
    // Loop through all the active windows
    for (const auto& windowsIter : mWindows)
    {
        const std::string& windowName = windowsIter.first;

        // Get the HWND handle
        Ogre::RenderWindow* renderWindow = windowsIter.second;
        HWND hWnd = nullptr;
        renderWindow->getCustomAttribute("WINDOW", &hWnd);

        poco_assert(hWnd != nullptr);
        poco_assert(GetPropW(hWnd, L"WINDOW_ORIGINAL_RESOLUTION") == nullptr);

        // Create a RECT structure to store the window size
        RECT* pWindowRect = new RECT();
        // Get the window client area width and height (stored in the 'right' and 'bottom' members)
        if (!GetClientRect(hWnd, pWindowRect))
        {
            DWORD dwResult = GetLastError();
            delete pWindowRect;
            throw Poco::RuntimeException(Poco::format("OgreApplication::SaveWindowResolutions - GetClientRect('%s') failed: Error: %#lx.", windowName, dwResult));
        }

        // Save the window resolution size with the window handle
        if (!SetPropW(hWnd, L"WINDOW_ORIGINAL_RESOLUTION", reinterpret_cast<HANDLE>(pWindowRect)))
        {
            DWORD dwResult = GetLastError();
            delete pWindowRect;
            throw Poco::RuntimeException(Poco::format("OgreApplication::SaveWindowResolutions - SetProp('%s') failed: Error: %#lx.", windowName, dwResult));
        }
    }
}

void OgreApplication::RestoreWindowResolutions()
{
    // Before restoring the window resolution sizes, flush the message queue to ensure
    // any pending messages based on the old resolution are processed
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        // Make sure the CEF browser is serviced
        CefDoMessageLoopWork();
    }

    // We need to ensure the the primary (main) window is always updated last,
    // else the windows will not display in the proper order (similar to Alt-Tab behavior)
    bool resolutionChanged = false;

    // Get the primary (main) window handle.
    HWND hwndMain = nullptr;
    RenderWindow* primaryRenderWindow = getPrimaryRenderWindow();
    primaryRenderWindow->getCustomAttribute("WINDOW", &hwndMain);

    // Get the primary (main) DirectX device surface if any
    IDirect3DDevice9* primaryDirect3DDevice9 = nullptr;
    primaryRenderWindow->getCustomAttribute("D3DDEVICE", &primaryDirect3DDevice9);

    // Loop through all the non-main active windows
    for (const auto& windowsIter : mWindows)
    {
        const std::string& windowName = windowsIter.first;

        // Get the HWND handle
        Ogre::RenderWindow* renderWindow = windowsIter.second;
        HWND hWnd = nullptr;
        renderWindow->getCustomAttribute("WINDOW", &hWnd);
        poco_assert(hWnd != nullptr);

        // Skip the the primary (main) window for last
        if (hWnd != hwndMain)
        {
            // Get the DirectX device surface if any and evict the resources
            IDirect3DDevice9* direct3DDevice9 = nullptr;
            renderWindow->getCustomAttribute("D3DDEVICE", &direct3DDevice9);
            if (direct3DDevice9 != nullptr)
            {
                direct3DDevice9->EvictManagedResources();
            }

            // Get the window size from the window handle and restore it
            RECT* pWindowRect = reinterpret_cast<RECT*>(RemovePropW(hWnd, L"WINDOW_ORIGINAL_RESOLUTION"));
            if (pWindowRect != nullptr)
            {
                RECT prevWindowRect;
                // Get the window client area width and height (stored in the 'right' and 'bottom' members)
                if (GetClientRect(hWnd, &prevWindowRect) &&
                    (prevWindowRect.right != pWindowRect->right ||
                        prevWindowRect.bottom != pWindowRect->bottom))
                {
                    // The RECT structure contains the original width and height of the client area in the 'right' and 'bottom' members.
                    ChangeWindowResolution(renderWindow,
                        pWindowRect->right,
                        pWindowRect->bottom);

                    resolutionChanged = true;
                }

                delete pWindowRect;
            }
            else
            {
                DWORD dwResult = GetLastError();
                throw Poco::RuntimeException(Poco::format("OgreApplication::RestoreWindowResolutions - RemovePropW('%s') failed: Error: %#lx.", windowName, dwResult));
            }
        }
    }

    // Restore the primary (main) window last
    if (primaryDirect3DDevice9 != nullptr)
    {
        primaryDirect3DDevice9->EvictManagedResources();
    }

    // Get the window size from the window handle and restore it
    RECT* pWindowRect = reinterpret_cast<RECT*>(RemovePropW(hwndMain, L"WINDOW_ORIGINAL_RESOLUTION"));
    if (pWindowRect != nullptr)
    {
        RECT prevWindowRect;
        // Get the window client area width and height (stored in the 'right' and 'bottom' members)
        if (GetClientRect(hwndMain, &prevWindowRect) &&
            (prevWindowRect.right != pWindowRect->right ||
                prevWindowRect.bottom != pWindowRect->bottom))
        {
            // The RECT structure contains the original width and height of the client area in the 'right' and 'bottom' members.
            ChangeWindowResolution(primaryRenderWindow,
                pWindowRect->right,
                pWindowRect->bottom);

            resolutionChanged = true;
        }
        delete pWindowRect;
    }
    else
    {
        DWORD dwResult = GetLastError();
        throw Poco::RuntimeException(Poco::format("OgreApplication::RestoreWindowResolutions - RemovePropW('MAIN') failed: Error: %#lx.", dwResult));
    }

    if (resolutionChanged)
    {
        //Force Ogre to update information for the rendertargets
        mRoot->renderOneFrame();

        //Simulate an alt tab to make sure the MAIN window is on top
        SimulateAltTab();
    }
}

void OgreApplication::ChangeWindowResolution(Ogre::RenderWindow* renderWindow, int width, int height)
{
    // see if we are fullscreen or not
    const MonitorConfig& monitorConfig = MonitorConfig::Instance();
    bool fullscreen = monitorConfig.GetFullscreen();

    // set the new resolution
    renderWindow->setFullscreen(fullscreen, width, height);

    // need to reposition the camera
    Ogre::Viewport* vp = getTopViewport(renderWindow->getName());

    Ogre::Real vpWidth = static_cast<Ogre::Real>(vp->getActualWidth());
    Ogre::Real vpHeight = static_cast<Ogre::Real>(vp->getActualHeight());

    Ogre::Real halfWidth = vpWidth * 0.5f;
    Ogre::Real halfHeight = vpHeight * 0.5f;

    float FOV = 45.0f;

    float fCameraDist = halfHeight / tan(Degree(FOV * 0.5f).valueRadians());

    Ogre::Camera* pCamera = vp->getCamera();

    pCamera->setPosition(Ogre::Vector3(halfWidth, -halfHeight, fCameraDist));

    pCamera->lookAt(Ogre::Vector3(halfWidth, -halfHeight, -300.0f));
    pCamera->setNearClipDistance(0.5f);
    pCamera->setAspectRatio(vpWidth / vpHeight);
    pCamera->setFOVy(Degree(FOV));

    // also need to get all of the overlayelements to recalculate their matrices
    std::vector<std::string> viewportOverlays = CustomizedOverlayManager::getSingleton().getViewportOverlays(vp);
    for (unsigned int i = 0; i < viewportOverlays.size(); i++)
    {
        Overlay* overlay = CustomizedOverlayManager::getSingleton().getByName(viewportOverlays[i]);

        Overlay::Overlay2DElementsIterator iElement = overlay->get2DElementsIterator();
        while (iElement.hasMoreElements())
        {
            OverlayElement* pElement = iElement.getNext();
            pElement->_positionsOutOfDate();
        }
    }
}

void OgreApplication::onPreloadAssets(const Json::Value& msg)
{
    const std::string themeId = msg[Json::StaticString("themeId")].asString();
    const std::string themeName = msg[Json::StaticString("themeName")].asString();

    // This doesn't really work anymore and should be removed.
    mPreloadManager.preloadTheme(themeId, &mLoadDependencyMgr);

    poco_debug(Poco::Logger::get(MODULENAME_GAMEENGINE), "Preloaded Assets for theme: " + themeName);

    Json::Value atlasPreloaded;
    Message::InitializeMessage("sys", "AtlasAssetsPreloaded", Message::EVENT, atlasPreloaded);
    atlasPreloaded["themeId"] = themeId;
    atlasPreloaded["themeName"] = themeName;
    SendEvent(atlasPreloaded);
}

void OgreApplication::switchToDefaultLanguage()
{
    const std::string defaultLanguage = MFORCE::Common::Localization::GetDefaultLanguage();
    const std::string currentLanguage = MFORCE::Common::Localization::GetLanguage();

    if (currentLanguage != defaultLanguage)
    {
        const std::string monetaryLanguage = MFORCE::Common::Localization::GetMonetaryLanguage();

        // Update Localization.
        MFORCE::Common::Localization::Initialize(defaultLanguage, monetaryLanguage);

        // Update the plugins.
        const Ogre::Root::PluginInstanceList& plugins = mRoot->getInstalledPlugins();
        std::for_each(plugins.begin(), plugins.end(), std::bind1st(std::mem_fun(&OgreApplication::PluginOnSetLanguage), this));
    }
}

void OgreApplication::onUnloadPreloadedTheme(const Json::Value& msg)
{
    mPreloadManager.unloadPreloadedTheme(msg);
    unloadButtonPanel();
}

MFORCE::Common::ResourceCatalog& OgreApplication::getPreloadResourceCatalog()
{
    return mPreloadResourceCatalog;
}

const PreloadInfo* OgreApplication::getPreloadInfo(const std::string& themeId) const
{
    return mPreloadManager.getPreloadThemeInfo(themeId);
}

bool OgreApplication::isMGUIPackage(const std::string& packageName) const
{
    return (packageName.find(PackagePrefix::MultiGamePackage) == 0);
}

bool OgreApplication::isSNGUIPackage(const std::string& packageName) const
{
    return (packageName.find(PackagePrefix::SitAndGoUIPackage) == 0);
}


const std::string OgreApplication::getPluginNameByPackageName(const std::string& packageName) const
{
    if (packageName.find(PackagePrefix::DLCPackage) == 0)
    {
        return PackageManagerPluginName::DLCPackage;
    }
    else if (packageName.find(PackagePrefix::MultiGamePackage) == 0)
    {
        return PackageManagerPluginName::MultiGamePackage;
    }
    else if (packageName.find(PackagePrefix::SitAndGoUIPackage) == 0)
    {
        return PackageManagerPluginName::SitAndGoUIPackage;
    }

    return "";
}


void OgreApplication::onPackageLoadCompletedEvent(const Json::Value& msg)
{
    const std::string& packageId = msg.get("packageId", "").asString();
    bool success = msg.get("success", false).asBool();

    mLoadDependencyMgr.setDependencyLoaded(packageId, success);

    std::string pluginName = getPluginNameByPackageName(packageId);
    PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(pluginName));

    if (pPackageManagerPlugin)
    {
        pPackageManagerPlugin->onPackageLoadComplete(packageId, success);
    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::onPackageLoadCompletedEvent() - couldn't find PackageManager plugin associated with packageId: " + packageId);
    }
}

void OgreApplication::onPackageUnloadCompletedEvent(const Json::Value& msg)
{
    const std::string& packageId = msg.get("packageId", "").asString();

    std::string pluginName = getPluginNameByPackageName(packageId);
    PackageManagerPluginBase* pPackageManagerPlugin = dynamic_cast<PackageManagerPluginBase*>(getPluginByName(pluginName));

    if (pPackageManagerPlugin)
    {
        pPackageManagerPlugin->onPackageUnloadComplete(packageId);
    }
    else
    {
        poco_error(Poco::Logger::get(MODULENAME_GAMEENGINE), "OgreApplication::onPackageUnloadCompletedEvent() - couldn't find PackageManager plugin associated with packageId: " + packageId);
    }
}

void OgreApplication::forceRenderRootFrame(const Ogre::Real timeSinceLastFrame)
{
    // Only allow the force rendering of the Ogre root if we're on the OgreApp thread... 
    //  This is a temporary fix until we find a better solution to not over starving the OgreApp thread
    if (mOgreAppThreadId == std::this_thread::get_id())
    {
        mRoot->renderOneFrame(timeSinceLastFrame);
    }
}

void OgreApplication::OnMainDoorNotification(const Json::Value& msg)
{
    bool isOpened = msg["isOpened"].asBool();
    if (mpCautionScene)
    {
        if (isOpened)
        {
            mpCautionScene->ShowCautionOverlays();
        }
        else
        {
            mpCautionScene->HideCautionOverlays();
        }
    }
}

void OgreApplication::createStitchingViewports(const std::string &stitchingType)
{
    Ogre::Viewport* mainViewport = getTopViewport("MAIN");
    CustomizedOverlayManager& overlayMgr = CustomizedOverlayManager::getSingleton();
    mStitchingOverlay.clear();  
    
    switch(MFORCE::G2S::Cabinet::GetStitchingMode(stitchingType))
    {
        case MFORCE::G2S::Cabinet::MainTwo_BPNone:
            if (MFORCE::G2S::Cabinet::GetTEMainMonitorResolutionType() == MFORCE::G2S::Cabinet::SquareDynastyVue)
            {
                // run HDX TE game on Vue
                mStitchingOverlay = "Core/DynastyVueLandscape";
                mVirtualWindowManager.createVirtualWindow("MAIN", StitchingWindow::width, StitchingWindow::height, "Core/DynastyVueLandscapeMainWindow");
                mVirtualWindowManager.createVirtualWindow("TOP", StitchingWindow::width, StitchingWindow::height, "Core/DynastyVueLandscapeTopWindow");
            }
            else
            {
                // run Vue TE game on Flex/Sol, create two virtual windows
                mStitchingOverlay = "Core/DynastyVueToTE43";
                mVirtualWindowManager.createVirtualWindow("MAIN", G2S::Cabinet::VueTEStitchingWindow::width, G2S::Cabinet::VueTEStitchingWindow::height, "Core/DynastyVueToTE43MainWindow");
                mVirtualWindowManager.createVirtualWindow("TOPSTITCHED", G2S::Cabinet::VueTEStitchingWindow::width, G2S::Cabinet::VueTEStitchingWindow::height, "Core/DynastyVueToTE43TopStitchedWindow");
            }
            break;
        case MFORCE::G2S::Cabinet::MainTwo_DynastyBP:
            // run HDX game on Vue, create two virtual windows
            mStitchingOverlay = "Core/DynastyVueLandscape";
            mVirtualWindowManager.createVirtualWindow("MAIN", StitchingWindow::width, StitchingWindow::height, "Core/DynastyVueLandscapeMainWindow");
            mVirtualWindowManager.createVirtualWindow("TOP", StitchingWindow::width, StitchingWindow::height, "Core/DynastyVueLandscapeTopWindow");
            break;
        case MFORCE::G2S::Cabinet::MainThree_BPNone:
            // run HDX game on Flex/Sol, create three virtual windows
            mStitchingOverlay = "Core/TE43";
            mVirtualWindowManager.createVirtualWindow("MAIN", StitchingWindow::width, StitchingWindow::height, "Core/TE43MainWindow");
            mVirtualWindowManager.createVirtualWindow("TOP", StitchingWindow::width, StitchingWindow::height, "Core/TE43TopWindow");
            break;
        default:
            break;
    }
    if (!mStitchingOverlay.empty())
        overlayMgr.showOverlay(mainViewport, mStitchingOverlay);
}

void OgreApplication::removeStitchingViewports(const std::string &stitchingType)
{
    if (!mStitchingOverlay.empty())
    {
        mVirtualWindowManager.destroyVirtualWindow("MAIN");
        switch (MFORCE::G2S::Cabinet::GetStitchingMode(stitchingType))
        {
            case MFORCE::G2S::Cabinet::MainTwo_DynastyBP:
            case MFORCE::G2S::Cabinet::MainThree_BPNone:
                mVirtualWindowManager.destroyVirtualWindow("TOP");
                break;
            case MFORCE::G2S::Cabinet::MainTwo_BPNone:
                if (MFORCE::G2S::Cabinet::GetTEMainMonitorResolutionType() == MFORCE::G2S::Cabinet::SquareDynastyVue)
                {
                    mVirtualWindowManager.destroyVirtualWindow("TOP");
                }
                else
                {
                    mVirtualWindowManager.destroyVirtualWindow("TOPSTITCHED");
                }
                break;
            default:
                break;
        }
        Ogre::Viewport* mainViewport = getTopViewport("MAIN");
        CustomizedOverlayManager& overlayMgr = CustomizedOverlayManager::getSingleton();
        overlayMgr.hideOverlay(mainViewport, mStitchingOverlay);
    }
}

const VirtualWindowManager& OgreApplication::getVirtualWindowManager() const
{
    return mVirtualWindowManager;
}

void OgreApplication::LoadComponentScripts(const std::string& themeId, const std::string& fallbackPath, const bool isAddon)
{
    // The Bootstrap call to v8::Debug::EnableAgent with a "wait for connect" value of
    // 'true' empirically doesn't work.  See my posting on the v8-users mailing list/group
    // at http://groups.google.com/group/v8-users/browse_thread/thread/61740714ab5237f2.
    // We need to give it a kick in the butt by invoking a V8 debug break command.
    bool debugwait = (Poco::NumberParser::parseUnsigned(mOptions["debugwait"]) != 0);

    std::string contextName = "";
    std::string searchPath = "";
    std::string scriptName = "";

    // Start component game scripts
    DataLayer::ThemeComponentList compList = GetComponentsToLoad(themeId);
    if (compList.size() > 0)
    {
#if defined(DEVELOPER)
        try
#endif
        {
            for (DataLayer::ThemeComponentList::const_iterator i = compList.begin(); i != compList.end(); ++i)
            {
                const DataLayer::ThemeComponent& gtc = *i;
                contextName = (gtc.IsBaseComponent) ? themeId : gtc.Name;

                if (isAddon)
                {
                    mpScriptEngine->CreateAddOnContext(contextName,
                        this,
                        (gtc.IsBaseComponent) ? ScriptEngine::StartFileType_Base : ScriptEngine::StartFileType_Component,
                        gtc.Args);
                }
                else
                {
                    mpScriptEngine->CreateContext(contextName,
                        this,
                        (gtc.IsBaseComponent) ? ScriptEngine::StartFileType_Base : ScriptEngine::StartFileType_Component,
                        debugwait, gtc.Args);
                }

                /// Load any libraries for this component
                for (int lib = 0; lib < gtc.LibraryPaths.size(); ++lib)
                {
                    Poco::Path libraryPath = PathInfo::Instance()->ParseTaggedPathString(gtc.LibraryPaths[lib], fallbackPath);
                    mpOgreLog->logMessage(Poco::format("Loading library from location %s", libraryPath.toString()));

                    if (libraryPath.isFile())
                    {
                        searchPath = libraryPath.parent().toString();
                        scriptName = libraryPath.getFileName();
                    }
                    else
                    {
                        searchPath = libraryPath.toString();
                        scriptName = "main.js";
                    }

                    mpScriptEngine->StartFile(contextName, searchPath, scriptName);
                }

                /// If the component doesn't specify a path, then assume it's the game dir
                Poco::Path componentPath = PathInfo::Instance()->ParseTaggedPathString(gtc.Path, fallbackPath);

                searchPath = componentPath.toString();
                scriptName = "main.js";
                mpOgreLog->logMessage(Poco::format("Loading Theme Component %s from location %s", contextName, searchPath));
                mpScriptEngine->StartFile(contextName, searchPath, scriptName);
                updateIdleSceneLoading();
            }
        }
#if defined(DEVELOPER)
        catch (MFORCE::Common::ResourceException& e)
        {
            throw MFORCE::Foundation::ScriptException(contextName, searchPath, scriptName, e.displayText());
        }
#endif
    }
}

void OgreApplication::UnLoadComponentScripts(const std::string& themeId, const bool forceGC, MFORCEScript::ScriptEngine::PurgeScriptObjectsDelegate purgeScriptObjectsDelegate)
{
    std::string contextName = "";

    // Start component game scripts
    DataLayer::ThemeComponentList compList = GetComponentsToLoad(themeId);
    if (compList.size() > 0)
    {
#if defined(DEVELOPER)
        try
#endif
        {
            for (DataLayer::ThemeComponentList::const_iterator i = compList.begin(); i != compList.end(); ++i)
            {
                const DataLayer::ThemeComponent& gtc = *i;
                contextName = (gtc.IsBaseComponent) ? themeId : gtc.Name;

                mpOgreLog->logMessage(Poco::format("Unloading Theme Component %s", contextName));
                mpScriptEngine->TerminateContext(contextName, false, purgeScriptObjectsDelegate);
            }
        }
#if defined(DEVELOPER)
        catch (MFORCE::Common::ResourceException& e)
        {
            throw MFORCE::Foundation::ScriptException(contextName + "\n" + e.displayText());
        }
#endif
    }
}

#if DEVELOPER
void OgreApplication::SetReelSymbolsHaveHalfStops(bool hasHalfStops)
{
    if (mSeeThruMgr)
    {
        mSeeThruMgr->SetReelSymbolsHaveHalfStops(hasHalfStops);
    }
}
#endif

void OgreApplication::LoadUnityMarquees(const std::string& themeName, const std::string& themePath)
{
    EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
    egmPlugin->LoadUnityMarquees(themeName, themePath);
}

void OgreApplication::UnloadUnityMarquees(const std::string& themeName)
{
    EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
    egmPlugin->UnloadUnityMarquees(themeName);
}

void OgreApplication::PlayUnityMarquee(const std::string& themeName, const std::string& scriptName, unsigned int loops, bool restart)
{
    EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
    egmPlugin->PlayUnityMarquee(themeName, scriptName, loops, restart);
}

void OgreApplication::loadCabinetMarqueeResource()
{
    // Register resources specific to a particular cabinet style.
    ResourceGroupManager& resourceGroundManager = ResourceGroupManager::getSingleton();
    Poco::Path resourcePath = MFORCE::G2S::Cabinet::GetMarqueeResourcePath();
    resourcePath.pushDirectory("Cabinet");
    resourceGroundManager.addResourceLocation(resourcePath.toString(), "FileSystem", MarqueeCoreAssetName, false);

    resourcePath = MFORCE::G2S::Cabinet::GetMarqueeResourcePath();
    resourcePath.pushDirectory("MechReels");
    resourceGroundManager.addResourceLocation(resourcePath.toString(), "FileSystem", MarqueeCoreAssetName, false);

    resourceGroundManager.initialiseResourceGroup(MarqueeCoreAssetName);
    resourceGroundManager.loadResourceGroup(MarqueeCoreAssetName);
}

void OgreApplication::unloadCabinetMarqueeResource()
{
    ResourceGroupManager& resourceGroundManager = ResourceGroupManager::getSingleton();
    if (resourceGroundManager.resourceGroupExists(MarqueeCoreAssetName))
    {
        resourceGroundManager.unloadResourceGroup(MarqueeCoreAssetName);
        resourceGroundManager.clearResourceGroup(MarqueeCoreAssetName);
        resourceGroundManager.destroyResourceGroup(MarqueeCoreAssetName);
    }
}

void OgreApplication::StopUnityMarquee(const std::string& themeName, const std::string& scriptName)
{
    EGMObjectsPlugin* egmPlugin = (EGMObjectsPlugin*)(getPluginByName("Plugin_EGMObjects"));
    egmPlugin->StopUnityMarquee(themeName, scriptName);
}


void OgreApplication::ScaleButtonPanelAndPositionCamera(Ogre::Real horizontalFactor, Ogre::Real verticalFactor)
{
    auto it = mViewports.find("BUTTON_PANEL");
    if (it == mViewports.end())
    {
        return;
    }
    auto pViewport = it->second;

    pViewport->setDimensions(0.0f, 0.0f, horizontalFactor,verticalFactor);
    Ogre::Real width = static_cast<Ogre::Real>(pViewport->getActualWidth());
    Ogre::Real height = static_cast<Ogre::Real>(pViewport->getActualHeight());

    Ogre::Real halfWidth = width * 0.5f;
    Ogre::Real halfHeight = height * 0.5f;

    float FOV = 45.0f;
    float fCameraDist = halfHeight / tan(Degree(FOV * 0.5f).valueRadians());

    Ogre::Camera* pCamera = mSceneMgr->getCamera("MasterUI_BUTTON_PANEL");
    pCamera->setPosition(Ogre::Vector3(halfWidth, -halfHeight, fCameraDist));

    pCamera->lookAt(Ogre::Vector3(halfWidth, -halfHeight, -300.0f));
    pCamera->setNearClipDistance(0.5f);
    pCamera->setAspectRatio(width / height);
    pCamera->setFOVy(Degree(FOV));
}

bool OgreApplication::ShouldScaleButtonPanel(Ogre::Real& horizontalScaleFactor, Ogre::Real& verticalScaleFactor, int& verticalDRRAdjustment)
{
    if (MFORCE::GPMenu::GetDynastyFamilyButtonsSettings().IsThemeRescalingKortekToEffinet(mActiveGame.ThemeId))
    {
        // these values work well when scaling from Dynasty Kortek 1920x448 BP to Effinet 1920x519 BP. 
        // Kortek has 1640 main area and rest belongs to the play button area. 1.19 constant was derived by dividing 1920 to 1640
        // and then averaging it so for all games look good
        horizontalScaleFactor = 1.19f;
        verticalScaleFactor = 1.0f;
        verticalDRRAdjustment = 45;
        return true;
    }

    return false;
}

void OgreApplication::SetIsGameEngineTypeUnity(bool gameEngineTypeUnity)
{ 
    mIsGameEngineTypeUnity = gameEngineTypeUnity;
    mKeyboardShortcuts.setIsUnityGameType(gameEngineTypeUnity);
}
