#include <assert.h>     /* assert */
#include <Windows.h>

#include "ResourceManager.h"

#define I_NUM_OF_SLOTS			5
#define I_NUM_OF_MASTER_KEYS	1

#define S_LOCKERSROOM			L"LockersRoom"
#define S_NUM_OF_SLOTS			L"NumOfSlots"
#define S_NUM_OF_MASTER_KEYS	L"NumOfMasterKeys"
#define S_INI_FILE_NAME			L"..\\LockersRoom.ini"

CResourceManager::CResourceManager(const SmartKeyType _SmartKeyType/* = stMagneticCard */)
	:	NumOfAvailableMasterKeys(GetPrivateProfileInt(S_LOCKERSROOM, S_NUM_OF_MASTER_KEYS, I_NUM_OF_MASTER_KEYS, S_INI_FILE_NAME)),
		MasterKey(KeyFactory(_SmartKeyType))
{
	const int NumOfSlots = GetPrivateProfileInt(S_LOCKERSROOM, S_NUM_OF_SLOTS, I_NUM_OF_SLOTS, S_INI_FILE_NAME);

	assert(NumOfSlots > 0);
	assert(NumOfAvailableMasterKeys > 0);

	for (int i = 1; i <= NumOfSlots; ++ i)
	{
		Slots.insert(std::pair<TSlotId, CSlot*>(i, new CSlot(i)));
		AvailabeKeys.insert(std::pair<CSmartKey*, TSlotId>(KeyFactory(_SmartKeyType), i));
	}
}

void CResourceManager::EraseAllKeys(MKeys& Keys)
{
	const MKeys::const_iterator itE(Keys.end());
	for (MKeys::const_iterator it = Keys.begin(); it != itE; ++ it)
		delete it->first;

	Keys.clear();
}

template<class K, class V> void CResourceManager::EraseAllPointerValues(std::map<K, V>& PointerValueMap)
{
	const std::map<K, V>::const_iterator itE(PointerValueMap.end());
	for (std::map<K, V>::const_iterator it = PointerValueMap.begin(); it != itE; ++ it)
		delete it->second;

	PointerValueMap.clear();
}

CResourceManager::~CResourceManager()
{
	EraseAllKeys(OccupiedKeys);
	EraseAllKeys(AvailabeKeys);
	EraseAllPointerValues<TSlotId, CSlot*>(Slots);
	EraseAllPointerValues<std::string, CUser*>(Users);

	delete MasterKey;
}

void CResourceManager::GiveKeyToUser(const std::string& _UserName)
{
	if (AvailabeKeys.empty())
	{
		puts ("No available key. Please wait and try again later.");
		return;
	}

	const MKeys::const_iterator itKey = AvailabeKeys.begin();
	CSmartKey* Key = itKey->first;
	const TSlotId SlotId(itKey->second);
	MUsers::const_iterator itUser = Users.find(_UserName);
	if (itUser == Users.end())
	{
		Users.insert(std::pair<std::string, CUser*>(_UserName, new CUser(Key, SlotId)));
		OccupiedKeys.insert(*itKey);
		AvailabeKeys.erase(itKey);
		std::cout << "User '" << _UserName << "' received a key for slot # " << SlotId << '\n';
		return;
	}

	CUser* User = itUser->second;
	const TSlotId UserSlotId = User->GetSlotId();
	if (UserSlotId != V_NULL_SLOT_ID && UserSlotId != SlotId)
	{
		std::cout << "User '" << _UserName << "' already have a key for slot # " << UserSlotId << '\n';
		return;
	}

	CSmartKey* UserKey = User->GetKey();
	if (UserKey != NULL && UserSlotId == V_SLOT_ID_ALL && UserKey->IsEqual(MasterKey))
		++ NumOfAvailableMasterKeys;

	User->TakeKey(Key, SlotId);
	OccupiedKeys.insert(*itKey);
	AvailabeKeys.erase(itKey);
	std::cout << "User '" << _UserName << "' received a key for slot # " << SlotId << '\n';
}

void CResourceManager::GiveMasterKeyToUser(const std::string& _UserName)
{
	if (NumOfAvailableMasterKeys == 0)
	{
		puts ("No available master key.");
		return;
	}

	MUsers::const_iterator itUser = Users.find(_UserName);
	if (itUser == Users.end())
		Users.insert(std::pair<std::string, CUser*>(_UserName, new CUser(MasterKey, V_SLOT_ID_ALL)));
	else
	{
		CSmartKey* UserKey = itUser->second->GetKey();
		MKeys::const_iterator itKey = OccupiedKeys.find(UserKey);
		if (itKey != OccupiedKeys.end())
		{
			AvailabeKeys.insert(*itKey);
			OccupiedKeys.erase(itKey);
		}
		itUser->second->TakeMasterKey(MasterKey);
	}

	-- NumOfAvailableMasterKeys;

	std::cout << "User '" << _UserName << "' received a master key.\n";
}

void CResourceManager::GetKeyFromUser(const std::string& _UserName)
{
	MUsers::const_iterator itUser = Users.find(_UserName);
	if (itUser == Users.end())
	{
		std::cout << "User '" << _UserName << "' not found\n";
		return;
	}

	CUser* User = itUser->second;
	CSmartKey* Key = User->GetKey();
	const TSlotId UserSlotId = User->GetSlotId();

	User->ReurnKey();

	if (Key->IsEqual(MasterKey))
	{
		std::cout << "Master key was returned by user '" << _UserName << ".\n";
		++ NumOfAvailableMasterKeys;
		return;
	}

	MKeys::const_iterator itKey = OccupiedKeys.find(Key);
	if (itKey == OccupiedKeys.end())
	{
		if (AvailabeKeys.find(Key) == AvailabeKeys.end())
			std::cout << "Warning: Key given to User '" << _UserName << "' was not found in our keys repositories\n";
		else
			std::cout << "Warning: Key given to User '" << _UserName << "' was not found in occupied keys repository\n";
		return;
	}

	AvailabeKeys.insert(*itKey);
	OccupiedKeys.erase(itKey);
	std::cout << "A key for slot # " << UserSlotId << " was returned by user '" << _UserName << ".\n";
}

bool CResourceManager::AuthenticateUser(const std::string& _UserName, TSlotId _SlotId/* = V_NULL_SLOT_ID*/) const
{
	MUsers::const_iterator itUser = Users.find(_UserName);
	if (itUser == Users.end())
	{
		std::cout << "Error: User '" << _UserName << "' does not have a key.\n";
		return false;
	}

	CUser* User = itUser->second;
	CSmartKey* UserKey = User->GetKey();
	if (UserKey == NULL)
	{
		std::cout << "Error: User '" << _UserName << "' does not have a key.\n";
		return false;
	}

	const TSlotId UserSlotId = User->GetSlotId();
	if (UserSlotId == V_SLOT_ID_ALL && UserKey->IsEqual(MasterKey))
		return true;

	MKeys::const_iterator itKey = OccupiedKeys.find(UserKey);
	if (itKey == OccupiedKeys.end())
	{
		std::cout << "Error: User '" << _UserName << "' does not have a key.\n";
		return false;
	}

	if (!UserKey->IsEqual(itKey->first))
	{
		std::cout << "Error: Users '" << _UserName << "' key doesn't fit.\n";
		return false;
	}
	
	if (itKey->second != UserSlotId)
	{
		std::cout << "Error: Users '" << _UserName << "' was assigned slot # " << UserSlotId << ". But his key fits slot # " << itKey->second << ".\n";
		return false;
	}
	
	if (_SlotId != V_NULL_SLOT_ID && _SlotId != UserSlotId)
	{
		std::cout << "Error: Users '" << _UserName << "' key doesn't fit slot # " << _SlotId << ". His key should fit slot # " << UserSlotId << ".\n";
		return false;
	}
	
	return true;
}

void CResourceManager::OpenDoorForUser(const std::string& _UserName)
{
	if (!AuthenticateUser(_UserName))
		return;

	Door.UnLock();
	std::cout << "User '" << _UserName << "', Welcome to the lockers room\n";
}

void CResourceManager::OpenSlotForUser(const std::string& _UserName, TSlotId _SlotId)
{
	if (!AuthenticateUser(_UserName, _SlotId))
		return;

	CSlot* Slot = Slots[_SlotId];
	Slot->UnLock();
	std::cout << "User '" << _UserName << "', Your slot # " << _SlotId << " is unlocked\n";
}

void CResourceManager::ListUsers()
{
	puts ("Users report.");
	if (Users.empty())
	{
		puts ("No users in the repository.");
		return;
	}

	const MUsers::const_iterator itE = Users.end();
	for (MUsers::const_iterator it = Users.begin(); it != itE; ++ it)
	{
		const TSlotId SlotId = it->second->GetSlotId();
		switch (SlotId)
		{
		case V_NULL_SLOT_ID:
			std::cout << "User '" << it->first << "' is not assigned a slot.\n";
			break;
		case V_SLOT_ID_ALL:
			std::cout << "User '" << it->first << "' is assigned the master key.\n";
			break;
		default:
			std::cout << "User '" << it->first << "' assigned to slot # " << it->second->GetSlotId() << ".\n";
			break;
		}
	}
}

void CResourceManager::PrintSlots(MKeys& Keys, const char* RepositoryName)
{
	if (Keys.empty())
		return;

	std::cout << RepositoryName << " slots # :\n";
	const MKeys::const_iterator itE = Keys.end();
	for (MKeys::const_iterator it = Keys.begin(); it != itE; ++ it)
		std::cout << it->second << " ";

	puts ("");
}

void CResourceManager::ListSlots()
{
	puts ("Slots report.");
	PrintSlots(OccupiedKeys, "Occupied");
	PrintSlots(AvailabeKeys, "Available");
}

