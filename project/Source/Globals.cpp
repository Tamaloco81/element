/*
    Globals.cpp - This file is part of Element
    Copyright (C) 2016 Kushview, LLC.  All rights reserved.
*/

#include "ElementApp.h"
#include "engine/InternalFormat.h"
#include "session/DeviceManager.h"
#include "session/MediaManager.h"
#include "session/PluginManager.h"
#include "session/Session.h"
#include "session/UnlockStatus.h"
#include "Settings.h"
#include "URIs.h"
#include "CommandManager.h"
#include "Globals.h"

namespace Element {

static void buildCommandLine (CommandLine& cli, const String& c) {
    cli.fullScreen = c.contains ("--full-screen");

    const var port = c.fromFirstOccurrenceOf("--port=", false, false)
                      .upToFirstOccurrenceOf(" ", false, false);
    if (port != var::null)
        cli.port = (int) port;
}

CommandLine::CommandLine (const String& c)
    : fullScreen (false),
      port (3123)
{
    if (c.isNotEmpty())
        buildCommandLine (*this, c);
}

class Globals::Impl
{
public:
    Impl (Globals& g)
        : owner (g) { }
    
    ~Impl() { }

    Globals& owner;
    AudioEnginePtr                engine;
    ScopedPointer<CommandManager> commands;
    ScopedPointer<DeviceManager>  devices;
    ScopedPointer<MediaManager>   media;
    ScopedPointer<PluginManager>  plugins;
    ScopedPointer<Settings>       settings;
    ScopedPointer<Session>        session;
    ScopedPointer<UnlockStatus>   unlock;
   #if !ELEMENT_LV2_PLUGIN_HOST
    ScopedPointer<SymbolMap>      symbols;
   #endif
   
private:
    friend class Globals;
    void init()
    {
#if !ELEMENT_LV2_PLUGIN_HOST
        symbols  = new SymbolMap();
#endif
        plugins  = new PluginManager();
        devices  = new DeviceManager();
        media    = new MediaManager();
        settings = new Settings();
        commands = new CommandManager();
        unlock   = new UnlockStatus (owner);
    }
    
    void freeAll()
    {
        commands = nullptr;
        session  = nullptr;
        devices  = nullptr;
        media    = nullptr;
        plugins  = nullptr;
        settings = nullptr;
        engine   = nullptr;
        unlock   = nullptr;
    }
};

Globals::Globals (const String& _cli)
    : WorldBase (this),
      cli (_cli)
{
    appName = "Element";
    impl = new Impl (*this);
    impl->init();
    impl->unlock->load();
}

Globals::~Globals()
{
    impl->freeAll();
}

CommandManager& Globals::getCommandManager()
{
    jassert (impl && impl->commands);
    return *impl->commands;
}

DeviceManager& Globals::getDeviceManager()
{
    jassert (impl->devices != nullptr);
    return *impl->devices;
}

MediaManager& Globals::getMediaManager()
{
    jassert (impl->media != nullptr);
    return *impl->media;
}

AudioEnginePtr Globals::getAudioEngine() const { return impl->engine; }

PluginManager& Globals::getPluginManager()
{
    jassert (impl->plugins != nullptr);
    return *impl->plugins;
}

Settings& Globals::getSettings()
{
    jassert (impl->settings != nullptr);
    return *impl->settings;
}

SymbolMap& Globals::getSymbolMap()
{
   #if ELEMENT_LV2_PLUGIN_HOST
    auto* fmt = impl->plugins->format<LV2PluginFormat>();
    jassert(fmt);
    return fmt->getSymbolMap();
   #else
    jassert(impl->symbols);
    return *impl->symbols;
   #endif
}

UnlockStatus& Globals::getUnlockStatus()
{
    jassert (impl && impl->unlock);
    return *impl->unlock;
}
    
Session& Globals::getSession()
{
    jassert (impl->session != nullptr);
    return *impl->session;
}

void Globals::setEngine (EnginePtr engine)
{
    if (impl->engine)
        impl->engine->deactivate();
    impl->engine = engine;
    if (impl->session == nullptr) {
        impl->session = new Session (*this);
    }

    getDeviceManager().attach (engine);
}

}
