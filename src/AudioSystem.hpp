//////////////////////////////////////////////////////////////////////////////
//
//            Copyright (c) 2017 Ronnie Brohn (EpicBrownie)      
//
//                Distributed under The MIT License (MIT).
//             (See accompanying file License.txt or copy at 
//                 https://opensource.org/licenses/MIT)
//
//           Please report any bugs, typos, or suggestions to
//              https://github.com/epicbrownie/Epic/issues
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Epic/AudioTypes.hpp>
#include <Epic/AudioBus.hpp>
#include <Epic/Event.hpp>
#include <Epic/OS.hpp>
#include <Epic/Sound.hpp>
#include <Epic/VolumeControl.hpp>
#include <Epic/StringHash.hpp>
#include <Epic/STL/List.hpp>
#include <Epic/STL/Map.hpp>
#include <Epic/STL/UniquePtr.hpp>
#include <Epic/detail/FMODInclude.hpp>
#include <Epic/detail/AudioAllocator.hpp>
#include <cassert>
#include <memory>
#include <type_traits>

//////////////////////////////////////////////////////////////////////////////

namespace Epic
{
	class AudioSystem;
}

//////////////////////////////////////////////////////////////////////////////

namespace Epic::detail
{
	namespace
	{
		void* __stdcall AudioAlloc(unsigned int sz, FMOD_MEMORY_TYPE /*type*/, const char* /* srcStr */)
		{
			Epic::detail::AudioAllocator<Epic::AudioSystem> allocator;

			Blk blk = allocator.Allocate(sz);
			if (blk)
			{
				auto pPrefix = allocator.Allocator().GetPrefixObject(blk, Epic::detail::AudioAlignment);
				pPrefix->Size = blk.Size;
			}

			return blk.Ptr;
		}

		void* __stdcall AudioRealloc(void* /*p*/, unsigned int /*sz*/, FMOD_MEMORY_TYPE /*type*/, const char* /* srcStr */)
		{
			// Force the audio system to handle the reallocation
			return nullptr;
		}

		void __stdcall AudioFree(void* p, FMOD_MEMORY_TYPE /*type*/, const char* /* srcStr */)
		{
			using Allocator = Epic::detail::AudioAllocator<Epic::AudioSystem>;
			Allocator allocator;

			Blk blk{ p, 1 };

			const auto pPrefix = allocator.Allocator().GetPrefixObject(blk, Epic::detail::AudioAlignment);
			blk.Size = pPrefix->Size;

			if constexpr (detail::CanDeallocate<Allocator>::value)
				allocator.Deallocate(blk);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

// AudioSystem
class Epic::AudioSystem
{
public:
	using Type = Epic::AudioSystem;

public:
	static constexpr int MaxChannels = 1024;

private:
	struct FMODRelease
	{
		void operator() (FMOD::Studio::System* pFMOD) const
		{
			if (pFMOD)
				pFMOD->release();
		}
	};

private:
	static constexpr int MaxPathLength = 256;

	using FMODStudioPtr = std::unique_ptr<FMOD::Studio::System, FMODRelease>;
	using SoundPtr = Epic::UniquePtr<Epic::Sound>;
	using BusPtr = Epic::UniquePtr<Epic::AudioBus>;
	using VolumeControlPtr = Epic::UniquePtr<Epic::VolumeControl>;
	
	using LibraryMap = Epic::STLUnorderedMap<Epic::StringHash, FMOD::Studio::Bank*>;
	using SoundMap = Epic::STLUnorderedMap<Epic::StringHash, FMOD::Studio::EventDescription*>;
	using BusMap = Epic::STLUnorderedMap<Epic::StringHash, FMOD::Studio::Bus*>;
	using VolumeCtrlMap = Epic::STLUnorderedMap<Epic::StringHash, FMOD::Studio::VCA*>;
	
	using LibraryList = Epic::STLList<FMOD::Studio::Bank*>;

private:
	FMODStudioPtr m_pFMOD;
	SoundMap m_Sounds;
	BusMap m_Buses;
	VolumeCtrlMap m_VolumeControls;
	LibraryMap m_Libraries;
	LibraryList m_PendingLibraries;

public:
	AudioSystem() noexcept
	{
		#ifdef WindowsOS
			::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		#endif
	}

	~AudioSystem() noexcept
	{
		#ifdef WindowsOS
			::CoUninitialize();
		#endif
	}

public:
	bool Initialize(Epic::eSpeakerMode speakerMode = Epic::eSpeakerMode::Default, bool enableProfiling = false) noexcept
	{
		// Assign allocation functions
		if (!FMODCHECK(
			FMOD::Memory_Initialize(
				nullptr, 0,
				&Epic::detail::AudioAlloc,
				&Epic::detail::AudioRealloc,
				&Epic::detail::AudioFree,
				FMOD_MEMORY_ALL)
		)) return false;

		// Create the FMOD studio system
		FMOD::Studio::System* pSystem = nullptr;

		if (!FMODCHECK(FMOD::Studio::System::create(&pSystem)))
			return false;

		m_pFMOD.reset(pSystem);
		m_pFMOD->setUserData(this);

		// Initialize output format
		FMOD::System* pFMOD = nullptr;
		m_pFMOD->getLowLevelSystem(&pFMOD);
		FMOD_SPEAKERMODE mode = FMOD_SPEAKERMODE_DEFAULT;

		switch (speakerMode)
		{
			default: 
			case eSpeakerMode::Default:  mode = FMOD_SPEAKERMODE_DEFAULT;  break;
			case eSpeakerMode::Mono:     mode = FMOD_SPEAKERMODE_MONO;     break;
			case eSpeakerMode::Stereo:   mode = FMOD_SPEAKERMODE_STEREO;   break;
			case eSpeakerMode::Quad:     mode = FMOD_SPEAKERMODE_QUAD;     break;
			case eSpeakerMode::Surround: mode = FMOD_SPEAKERMODE_SURROUND; break;
			case eSpeakerMode::FiveOne:  mode = FMOD_SPEAKERMODE_5POINT1;  break;
			case eSpeakerMode::SevenOne: mode = FMOD_SPEAKERMODE_7POINT1;  break;
		}

		if (!FMODCHECK(pFMOD->setSoftwareFormat(0, mode, 0)))
			return false;

		// Initialize FMOD Studio, which will also initialize FMOD Low Level
		FMOD_STUDIO_INITFLAGS fmodStudioFlags = FMOD_STUDIO_INIT_NORMAL;
		FMOD_INITFLAGS fmodFlags = FMOD_INIT_NORMAL;
		if (enableProfiling) fmodStudioFlags |= FMOD_INIT_PROFILE_ENABLE;

		if (!FMODCHECK(m_pFMOD->initialize(MaxChannels, fmodStudioFlags, fmodFlags, nullptr)))
			return false;

		return true;
	}

private:
	#pragma region Private Helpers

	bool AddEvent(FMOD::Studio::EventDescription* pEvent) noexcept
	{
		// Create an event id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pEvent->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Store the event desc
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		m_Sounds[id] = pEvent;

		// Notify success
		this->SoundAdded(buffer, id);

		return true;
	}

	bool RemoveEvent(FMOD::Studio::EventDescription* pEvent) noexcept
	{
		// Create an event id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pEvent->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Remove the event handle
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		auto it = m_Sounds.find(id);

		if (it != std::end(m_Sounds))
		{
			m_Sounds.erase(id);

			// Notify success
			this->SoundRemoved(buffer, id);
		}

		return true;
	}

	bool LoadEvents(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the events in the bank
		int eventCount = 0;
		pBank->getEventCount(&eventCount);

		if (eventCount < 1)
			return true;

		auto freeEvents = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::EventDescription*, decltype(freeEvents)> pEvents
		{
			reinterpret_cast<FMOD::Studio::EventDescription**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::EventDescription*) * eventCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeEvents
		};

		if (!FMODCHECK(pBank->getEventList(pEvents.get(), eventCount, &eventCount)))
			return false;

		for (int i = 0; i < eventCount; ++i)
		{
			if (!AddEvent(pEvents.get()[i]))
				return false;
		}

		return true;
	}

	bool UnloadEvents(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the events in the bank
		int eventCount = 0;
		pBank->getEventCount(&eventCount);

		if (eventCount < 1)
			return true;

		auto freeEvents = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::EventDescription*, decltype(freeEvents)> pEvents
		{
			reinterpret_cast<FMOD::Studio::EventDescription**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::EventDescription*) * eventCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeEvents
		};

		if (!FMODCHECK(pBank->getEventList(pEvents.get(), eventCount, &eventCount)))
			return false;

		for (int i = 0; i < eventCount; ++i)
			RemoveEvent(pEvents.get()[i]);

		return true;
	}

	bool AddBus(FMOD::Studio::Bus* pBus) noexcept
	{
		// Create a bus id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pBus->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Store the bus
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		m_Buses[id] = pBus;

		// Notify success
		this->BusAdded(buffer, id);

		return true;
	}

	bool RemoveBus(FMOD::Studio::Bus* pBus) noexcept
	{
		// Create a bus id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pBus->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Remove the bus handle
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		auto it = m_Buses.find(id);

		if (it != std::end(m_Buses))
		{
			m_Buses.erase(id);

			// Notify success
			this->BusRemoved(buffer, id);
		}

		return true;
	}

	bool LoadBuses(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the buses in the bank
		int busCount = 0;
		pBank->getBusCount(&busCount);

		if (busCount < 1)
			return true;

		auto freeBuses = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::Bus*, decltype(freeBuses)> pBuses
		{
			reinterpret_cast<FMOD::Studio::Bus**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::Bus*) * busCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeBuses
		};

		if (!FMODCHECK(pBank->getBusList(pBuses.get(), busCount, &busCount)))
			return false;

		for (int i = 0; i < busCount; ++i)
		{
			if (!AddBus(pBuses.get()[i]))
				return false;
		}

		return true;
	}

	bool UnloadBuses(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the buses in the bank
		int busCount = 0;
		pBank->getBusCount(&busCount);

		if (busCount < 1)
			return true;

		auto freeBuses = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::Bus*, decltype(freeBuses)> pBuses
		{
			reinterpret_cast<FMOD::Studio::Bus**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::Bus*) * busCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeBuses
		};

		if (!FMODCHECK(pBank->getBusList(pBuses.get(), busCount, &busCount)))
			return false;

		for (int i = 0; i < busCount; ++i)
			RemoveBus(pBuses.get()[i]);

		return true;
	}

	bool AddVolumeControl(FMOD::Studio::VCA* pVolumeControl) noexcept
	{
		// Create a VCA id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pVolumeControl->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Store the VCA handle
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		m_VolumeControls[id] = pVolumeControl;

		// Notify success
		this->VolumeControlAdded(buffer, id);

		return true;
	}

	bool RemoveVolumeControl(FMOD::Studio::VCA* pVolumeControl) noexcept
	{
		// Create a VCA id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pVolumeControl->getPath(buffer, MaxPathLength, &length)))
			return false;

		// Remove the VCA handle
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		auto it = m_VolumeControls.find(id);

		if (it != std::end(m_VolumeControls))
		{
			m_VolumeControls.erase(id);

			// Notify success
			this->VolumeControlRemoved(buffer, id);
		}

		return true;
	}

	bool LoadVolumeControls(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the VCAs in the bank
		int VolumeControlCount = 0;
		pBank->getVCACount(&VolumeControlCount);

		if (VolumeControlCount < 1)
			return true;

		auto freeVolumeControls = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::VCA*, decltype(freeVolumeControls)> pVolumeControls
		{
			reinterpret_cast<FMOD::Studio::VCA**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::VCA*) * VolumeControlCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeVolumeControls
		};

		if (!FMODCHECK(pBank->getVCAList(pVolumeControls.get(), VolumeControlCount, &VolumeControlCount)))
			return false;

		for (int i = 0; i < VolumeControlCount; ++i)
		{
			if (!AddVolumeControl(pVolumeControls.get()[i]))
				return false;
		}

		return true;
	}

	bool UnloadVolumeControls(FMOD::Studio::Bank* pBank) noexcept
	{
		// Load the VolumeControls in the bank
		int VolumeControlCount = 0;
		pBank->getVCACount(&VolumeControlCount);

		if (VolumeControlCount < 1)
			return true;

		auto freeVolumeControls = [](auto* p) noexcept
		{
			Epic::detail::AudioFree(p, FMOD_MEMORY_NORMAL, nullptr);
		};

		std::unique_ptr<FMOD::Studio::VCA*, decltype(freeVolumeControls)> pVolumeControls
		{
			reinterpret_cast<FMOD::Studio::VCA**>(Epic::detail::AudioAlloc(
				sizeof(FMOD::Studio::VCA*) * VolumeControlCount,
				FMOD_MEMORY_NORMAL,
				nullptr)),
			freeVolumeControls
		};

		if (!FMODCHECK(pBank->getVCAList(pVolumeControls.get(), VolumeControlCount, &VolumeControlCount)))
			return false;

		for (int i = 0; i < VolumeControlCount; ++i)
			RemoveVolumeControl(pVolumeControls.get()[i]);

		return true;
	}

	Epic::StringHash ProcessBank(FMOD::Studio::Bank* pBank) noexcept
	{
		// Create a bank id
		char buffer[MaxPathLength];
		int length = 0;

		if (!FMODCHECK(pBank->getPath(buffer, MaxPathLength, &length)) || length == 0)
			return Epic::Hash("");

		// Add bank events to sound map
		if (!LoadEvents(pBank))
			return Epic::Hash("");

		// Add buses to bus map
		if (!LoadBuses(pBank))
			return Epic::Hash("");

		// Add VCAs to the volume control map
		if (!LoadVolumeControls(pBank))
			return Epic::Hash("");

		// Store the library handle
		Epic::StringHash id = Epic::Hash(reinterpret_cast<const char*>(buffer));
		m_Libraries[id] = pBank;

		// Notify success
		this->LibraryAdded(buffer, id);

		return id;
	}

	bool UnloadBank(FMOD::Studio::Bank* pBank) noexcept
	{
		bool success = true;

		// Remove volume controls
		success = success && UnloadVolumeControls(pBank);

		// Remove buses
		success = success && UnloadBuses(pBank);

		// Remove sounds
		success = success && UnloadEvents(pBank);

		// Unload the bank
		success = success && FMODCHECK(pBank->unload());

		return success;
	}

	#pragma endregion

public: 
	#pragma region Libraries

	bool LoadAudioLibrary(const char* pFilePath) noexcept
	{
		assert(m_pFMOD);

		// Load the bank		
		FMOD::Studio::Bank* pBank = nullptr;

		auto result = m_pFMOD->loadBankFile(pFilePath, FMOD_STUDIO_LOAD_BANK_NORMAL, &pBank);
		if (result != FMOD_OK)
		{
			FMODCHECK(result);
			return (result == FMOD_ERR_EVENT_ALREADY_LOADED);
		}

		// Add the bank to library map
		auto id = ProcessBank(pBank);
		if (id == Epic::Hash(""))
		{
			UnloadBank(pBank);
			return false;
		}

		return true;
	}

	bool LoadAudioLibrary(const char* pBuffer, size_t length) noexcept
	{
		assert(m_pFMOD);

		// Load the bank		
		FMOD::Studio::Bank* pBank = nullptr;

		auto result = m_pFMOD->loadBankMemory(pBuffer, static_cast<int>(length), FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NORMAL, &pBank);
		if (result != FMOD_OK)
		{
			FMODCHECK(result);
			return (result == FMOD_ERR_EVENT_ALREADY_LOADED);
		}

		// Add the bank to library map
		auto id = ProcessBank(pBank);
		if (id == Epic::Hash(""))
		{
			UnloadBank(pBank);
			return false;
		}

		return true;
	}

	bool LoadAudioLibraryAsync(const char* pFilePath) noexcept
	{
		assert(m_pFMOD);

		// Load the bank		
		FMOD::Studio::Bank* pBank = nullptr;

		auto result = m_pFMOD->loadBankFile(pFilePath, FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &pBank);
		if (result != FMOD_OK)
		{
			FMODCHECK(result);
			return (result == FMOD_ERR_EVENT_ALREADY_LOADED);
		}

		// Add the bank to pending list
		m_PendingLibraries.emplace_back(pBank);

		return true;
	}

	bool LoadAudioLibraryAsync(const char* pBuffer, size_t length) noexcept
	{
		assert(m_pFMOD);

		// Load the bank		
		FMOD::Studio::Bank* pBank = nullptr;

		auto result = m_pFMOD->loadBankMemory(pBuffer, static_cast<int>(length), FMOD_STUDIO_LOAD_MEMORY, FMOD_STUDIO_LOAD_BANK_NONBLOCKING, &pBank);
		if (result != FMOD_OK)
		{
			FMODCHECK(result);
			return (result == FMOD_ERR_EVENT_ALREADY_LOADED);
		}

		// Add the bank to pending list
		m_PendingLibraries.emplace_back(pBank);

		return true;
	}

	void UnloadAudioLibrary(const Epic::StringHash libraryID) noexcept
	{
		auto it = m_Libraries.find(libraryID);
		if (it != std::end(m_Libraries))
		{
			auto pBank = (*it).second;

			// Unload all events, buses, etc... for this bank
			UnloadBank(pBank);

			// Remove bank
			m_Libraries.erase(libraryID);
			
			LibraryRemoved(nullptr, libraryID);
		}
	}

	void CacheLibrary(const Epic::StringHash libraryID) noexcept
	{
		auto it = m_Libraries.find(libraryID);
		if (it != std::end(m_Libraries))
		{
			auto pBank = (*it).second;
			FMODCHECK(pBank->loadSampleData());
		}
	}

	void UncacheLibrary(const Epic::StringHash libraryID) noexcept
	{
		auto it = m_Libraries.find(libraryID);
		if (it != std::end(m_Libraries))
		{
			auto pBank = (*it).second;
			FMODCHECK(pBank->unloadSampleData());
		}
	}

	#pragma endregion

public:
	#pragma region Sounds
		
	SoundPtr CreateInstance(const Epic::StringHash id) noexcept
	{
		auto it = m_Sounds.find(id);
		if (it != std::end(m_Sounds))
			return Epic::MakeUnique<Epic::Sound>((*it).second);

		return nullptr;
	}

	void CacheSound(const Epic::StringHash soundID) noexcept
	{
		auto it = m_Sounds.find(soundID);
		if (it != std::end(m_Sounds))
		{
			auto pEvent = (*it).second;
			FMODCHECK(pEvent->loadSampleData());
		}
	}

	void UncacheSound(const Epic::StringHash soundID) noexcept
	{
		auto it = m_Sounds.find(soundID);
		if (it != std::end(m_Sounds))
		{
			auto pEvent = (*it).second;
			FMODCHECK(pEvent->unloadSampleData());
		}
	}

	#pragma endregion

public:
	#pragma region Buses

	BusPtr GetBus(const Epic::StringHash id) noexcept
	{
		auto it = m_Buses.find(id);
		if (it != std::end(m_Buses))
			return Epic::MakeUnique<Epic::AudioBus>((*it).second);

		return nullptr;
	}

	#pragma endregion

public:
	#pragma region Volume Controls

	VolumeControlPtr GetVolumeControl(const Epic::StringHash id) noexcept
	{
		auto it = m_VolumeControls.find(id);
		if (it != std::end(m_VolumeControls))
			return Epic::MakeUnique<Epic::VolumeControl>((*it).second);

		return nullptr;
	}

	#pragma endregion

public:
	void Update() noexcept
	{
		assert(m_pFMOD);

		// Update studio
		FMODCHECK(m_pFMOD->update());

		// Update pending library loads
		auto itBank = std::begin(m_PendingLibraries);
		while (itBank != std::end(m_PendingLibraries))
		{
			auto pBank = *itBank;
			auto itNext = std::next(itBank);

			FMOD_STUDIO_LOADING_STATE state;
			auto result = pBank->getLoadingState(&state);
			
			if (state == FMOD_STUDIO_LOADING_STATE_LOADING)
			{
				// The bank is still loading
				continue;
			}
			else if (state == FMOD_STUDIO_LOADING_STATE_LOADED)
			{
				// The bank has loaded successfully
				m_PendingLibraries.erase(itBank);

				// Add the bank to library map
				auto id = ProcessBank(pBank);
				if (id == Epic::Hash(""))
				{
					UnloadBank(pBank);
					continue;
				}
			}
			else
			{
				// An error occurred while loading
				m_PendingLibraries.erase(itBank);
				FMODCHECK(result);
			}

			itBank = itNext;
		}
	}

private:
	using AvailableDelegate = Epic::Event<void(const char*, const Epic::StringHash)>;

public:
	AvailableDelegate LibraryAdded;
	AvailableDelegate LibraryRemoved;
	AvailableDelegate SoundAdded;
	AvailableDelegate SoundRemoved;
	AvailableDelegate BusAdded;
	AvailableDelegate BusRemoved;
	AvailableDelegate VolumeControlAdded;
	AvailableDelegate VolumeControlRemoved;
};