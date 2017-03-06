#include <exception>
#include "SmartKey.h"

bool MagneticCard::IsEqual(CSmartKey* OtherSmartKey) const
{
	MagneticCard* OtherMagneticCard(dynamic_cast<MagneticCard*>(OtherSmartKey));
	if (OtherMagneticCard == NULL)
	{
		ENonMagneticCardObjectException NonMagneticCardObjectException;
		throw NonMagneticCardObjectException;
	}

	return CardId == OtherMagneticCard->CardId;
}
