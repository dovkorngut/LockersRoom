#ifndef USER_H
#define USER_H

#include "Slot.h"

class CUser
{
private:
	CSmartKey* Key;
	TSlotId SlotId;

public:
	CUser(CSmartKey* _Key = NULL, const TSlotId _SlotId = V_NULL_SLOT_ID) : Key(_Key), SlotId(_SlotId) {}
	~CUser() { }

	void TakeKey(CSmartKey* _Key, const TSlotId _SlotId) { Key = _Key; SlotId = _SlotId; }
	void TakeMasterKey(CSmartKey* _Key) { Key = _Key; SlotId = V_SLOT_ID_ALL; }
	void ReurnKey() { Key = NULL; SlotId = V_NULL_SLOT_ID; }

	CSmartKey* GetKey() const { return Key; }
	TSlotId GetSlotId() const { return SlotId; }
};

#endif