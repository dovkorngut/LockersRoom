#ifndef SMARTKEY_H
#define SMARTKEY_H

#include <stdlib.h> /* srand, rand */

class CSmartKey
{
public:
	virtual bool IsEqual(CSmartKey* OtherSmartKey) const = 0;
};

class ENonMagneticCardObjectException: public std::exception
{
  virtual const char* what() const throw()
  {
    return "Error! A 'CSmartKey' object of other than 'MagneticCard' type was received. Application will terminate!";
  }
};

typedef int TCardId;

class MagneticCard : public CSmartKey
{
private:
	const TCardId CardId;

public:
	MagneticCard() : CardId(rand() % 1000000) { } // Burn Id in the range 0 to 999999
	~MagneticCard() { }

	bool IsEqual(CSmartKey* OtherSmartKey) const;
};

#endif