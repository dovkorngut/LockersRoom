#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <string>
#include <map>
#include <iostream>

#include "SmartKey.h"
#include "Slot.h"
#include "User.h"

enum SmartKeyType { stUnknown, stMagneticCard };

typedef std::map<TSlotId, CSlot*> MSlots;		// SlotId	=> Slot
typedef std::map<std::string, CUser*> MUsers;	// UserName	=> User
typedef std::map<CSmartKey*, TSlotId> MKeys;	// Key		=> SlotId

class CResourceManager
{
private:
	int NumOfAvailableMasterKeys;

	CSmartKey* MasterKey;

	CDoor Door;
	MSlots Slots;		// SlotId	=> Slot
	MUsers Users;		// UserName	=> User
	MKeys OccupiedKeys;	// Key		=> SlotId
	MKeys AvailabeKeys;	// Key		=> SlotId

	CSmartKey* KeyFactory(const SmartKeyType _SmartKeyType = stMagneticCard) const
		{ return (_SmartKeyType == stMagneticCard) ? new MagneticCard() : NULL; }

	template<class K, class V> void EraseAllPointerValues(std::map<K,V>& PointerValueMap);
	void EraseAllKeys(std::map<CSmartKey*, TSlotId>& Keys);
	bool AuthenticateUser(const std::string& _UserName, TSlotId _SlotId = V_NULL_SLOT_ID) const;
	void PrintSlots(std::map<CSmartKey*, TSlotId>& Keys, const char* RepositoryName);

public:
	CResourceManager(const SmartKeyType _SmartKeyType = stMagneticCard);
	~CResourceManager();

	void GiveKeyToUser(const std::string& _UserName);
	void GiveMasterKeyToUser(const std::string& _UserName);
	void GetKeyFromUser(const std::string& _UserName);
	void OpenDoorForUser(const std::string& _UserName);
	void OpenSlotForUser(const std::string& _UserName, TSlotId _SlotId);

	void ListUsers();
	void ListSlots();
};

#endif