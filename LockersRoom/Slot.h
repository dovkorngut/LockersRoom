#ifndef SLOT_H
#define SLOT_H

#define V_NULL_SLOT_ID -1
#define V_SLOT_ID_ALL	0

typedef int TSlotId;

enum ELockedStatus { dsLocked, dsUnlocked };

/*
** This is an operatinal class that may comunicate with the device
** accessed exlusively via the ResourceManager and shold not be passed to other entities.
*/
class LockableItem
{
protected:
	ELockedStatus LockedStatus;
	
public:
	virtual void UnLock() = 0;
	virtual void Lock() = 0;
};

class CSlot : public LockableItem
{
private:
	const TSlotId SlotId;

public:
	CSlot(const int _SlotId) : SlotId(_SlotId) {}
	~CSlot() { }

	bool IsSlotId(const TSlotId _SlotId) { return _SlotId == SlotId; }
	virtual void UnLock() override { if (LockedStatus == dsLocked) LockedStatus = dsUnlocked; }
	virtual void Lock() override { if (LockedStatus == dsUnlocked) LockedStatus = dsLocked; }
};

class CDoor : public LockableItem
{
public:
	CDoor() { }
	~CDoor() { }

	virtual void UnLock() override { if (LockedStatus == dsLocked) LockedStatus = dsUnlocked; }
	virtual void Lock() override { if (LockedStatus == dsUnlocked) LockedStatus = dsLocked; }
};

#endif